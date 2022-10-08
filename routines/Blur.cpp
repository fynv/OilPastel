#include <QOpenGLExtraFunctions>
#include <string>
#include "Blur.h"

static std::string g_compute =
R"(#version 430

// THREADS_PER_GROUP
// SHARED_MEM_SIZE
// DATA_SIZE
#DEFINES#

#define TEXELS_PER_THREAD  ( (THREADS_PER_GROUP-1+DATA_SIZE)/THREADS_PER_GROUP )
#define	SCAN_WARP_SIZE 32
#define SCAN_LOG2_WARP_SIZE	5

layout(local_size_x =THREADS_PER_GROUP, local_size_y = 1) in;

layout(binding=0, r32f) uniform image2D image;

shared float shared_data[SHARED_MEM_SIZE];

layout (location = 0) uniform int g_NumApproxPasses;
layout (location = 1) uniform float g_HalfBoxFilterWidth;
layout (location = 2) uniform float g_FracHalfBoxFilterWidth;
layout (location = 3) uniform float g_InvFracHalfBoxFilterWidth;
layout (location = 4) uniform float g_RcpBoxFilterWidth;
layout (location = 5) uniform float g_Radius;
layout (location = 6) uniform float g_Contrast;

float scan_warp_inclusive(int thread_id, float value, int size)
{    
    uint location = 2 * thread_id - (thread_id & (size - 1));
    
    shared_data[location] = 0.0;
    location += size;
    shared_data[location] = value;

    for (uint offset = 1; offset < size; offset <<= 1)
        shared_data[location] += shared_data[location - offset];

    return shared_data[location];
}

float scan_warp_exclusive(int thread_id, float value, int size)
{
    return scan_warp_inclusive(thread_id, value, size) - value;
}


float scan_top_inclusive(int thread_id, float value, int size)
{    
    float warp_result = scan_warp_inclusive(thread_id, value, SCAN_WARP_SIZE);
 
    barrier();
    
    if ((thread_id & (SCAN_WARP_SIZE - 1)) == SCAN_WARP_SIZE - 1)
        shared_data[thread_id >> SCAN_LOG2_WARP_SIZE] = warp_result;
    barrier();

    float top_value = shared_data[thread_id];

    if( thread_id < SCAN_WARP_SIZE )
        shared_data[thread_id] = scan_warp_exclusive(thread_id, top_value, THREADS_PER_GROUP >> SCAN_LOG2_WARP_SIZE);

    barrier();

    return warp_result + shared_data[thread_id >> SCAN_LOG2_WARP_SIZE];
}

float scan_top_exclusive(int thread_id, float value, int size)
{
    return scan_top_inclusive(thread_id, value, size) - value;
}


void scan_inclusive( int thread_id , int size )
{
    uint i;
    uint location = thread_id * TEXELS_PER_THREAD;    
    float local_data[TEXELS_PER_THREAD];
    
    for (i = 0; i < TEXELS_PER_THREAD; i ++)
        local_data[i] = location + i < size ? shared_data[location + i] : 0.0;
    
    for (i = 1; i < TEXELS_PER_THREAD; i ++)
        local_data[i] += local_data[i - 1];
    
    barrier();

    float top_value = local_data[TEXELS_PER_THREAD - 1];
    float top_result = scan_top_exclusive(thread_id, top_value , THREADS_PER_GROUP);

    barrier();

    for (i = 0; i < TEXELS_PER_THREAD; i ++)
    {
        if (location + i < size)
            shared_data[location + i] = local_data[i] + top_result;
    }
    
    barrier();
}


void scan_inclusive_filtering(int thread_id, int size)
{
    int i;

    int location = thread_id * TEXELS_PER_THREAD;    
    float local_data[TEXELS_PER_THREAD];

    int L_pos = int( ceil(location - 0.5 - g_HalfBoxFilterWidth) - 1);
    int R_pos = int(floor(location - 0.5 + g_HalfBoxFilterWidth));

    float L_sum = shared_data[clamp(L_pos, 0, int(size - 1))] * g_FracHalfBoxFilterWidth;
    float R_sum = shared_data[clamp(R_pos, 0, int(size - 1))] * g_InvFracHalfBoxFilterWidth;

    for (i = 0; i < TEXELS_PER_THREAD; i ++)
    {
        float L_next = shared_data[clamp(L_pos + 1 + i, 0, int(size - 1))];
        float R_next = shared_data[clamp(R_pos + 1 + i, 0, int(size - 1))];

        local_data[i] = (R_sum + R_next * g_FracHalfBoxFilterWidth) - (L_sum + L_next * g_InvFracHalfBoxFilterWidth);
        local_data[i] *= g_RcpBoxFilterWidth;

        L_sum += L_next;
        R_sum += R_next;
    }

    barrier();

    float top_value = local_data[TEXELS_PER_THREAD - 1];
    float top_result = scan_top_exclusive(thread_id, top_value, THREADS_PER_GROUP);

    barrier();

    for (i = 0; i < TEXELS_PER_THREAD; i ++)
    {
        if (location + i < size)
            shared_data[location + i] = local_data[i] + top_result;
    }

    barrier();
}


float box_filtering(int location, int size)
{    
    float center = location - 0.5;
    int L_a = clamp(int(ceil(center - g_HalfBoxFilterWidth)), 0, size - 1);
    int L_b = clamp(L_a - 1, 0, size - 1);
    int R_a = clamp(int(floor(center + g_HalfBoxFilterWidth)), 0, size - 1);
    int R_b = clamp(R_a + 1, 0, size - 1);
    
    float L_value = mix(shared_data[L_a], shared_data[L_b], g_FracHalfBoxFilterWidth);
    float R_value = mix(shared_data[R_a], shared_data[R_b], g_FracHalfBoxFilterWidth);

    return (R_value - L_value) * g_RcpBoxFilterWidth;
}

#INPUT_OUTPUT#

void main()
{
	int group_id = int(gl_WorkGroupID.x);
    int thread_id = int(gl_LocalInvocationID.x);

	input_color( group_id , thread_id );
    scan_inclusive( thread_id , DATA_SIZE );

    for (uint i = 0; i < g_NumApproxPasses; i ++)
        scan_inclusive_filtering( thread_id , DATA_SIZE );

    output_color( group_id , thread_id );
}
)";

static std::string g_row_io =
R"(
void input_color( int gid , int tid )
{
    int col = tid;
    int row = gid;

    while( col < DATA_SIZE  )
    {
		shared_data[col] = imageLoad(image, ivec2(col,row)).x;
        col += THREADS_PER_GROUP;
    }
    
    barrier();
}


void output_color( int gid , int tid )
{
    int col = tid;
    int row = gid;

    while( col < DATA_SIZE )
    {
        float color = box_filtering(col, DATA_SIZE);
        color = clamp(0.5 + (color - 0.5)* g_Radius * g_Contrast, 0.0, 1.0);        
        imageStore(image, ivec2(col,row), vec4( color , color , color , 1.0 ) );		
        col += THREADS_PER_GROUP;
    }
}
)";

static std::string g_col_io =
R"(
void input_color( int gid , int tid )
{
    int col = gid;
    int row = tid;

    while( row < DATA_SIZE  )
    {
		shared_data[row] = imageLoad(image, ivec2(col,row)).x;
        row += THREADS_PER_GROUP;
    }    
    barrier();
}

void output_color( int gid , int tid )
{
    int col = gid;
    int row = tid;

    while( row < DATA_SIZE )
    {
        float color = box_filtering(row, DATA_SIZE);
        color = clamp(0.5 + (color - 0.5)* g_Radius * g_Contrast, 0.0, 1.0);
		imageStore(image, ivec2(col,row), vec4( color , color , color , 1.0 ) );
        row += THREADS_PER_GROUP;
    }
}
)";


inline void replace(std::string& str, const char* target, const char* source)
{
    int start = 0;
    size_t target_len = strlen(target);
    size_t source_len = strlen(source);
    while (true)
    {
        size_t pos = str.find(target, start);
        if (pos == std::string::npos) break;
        str.replace(pos, target_len, source);
        start = pos + source_len;
    }
}

inline float calcBoxFilterWidth(float radius, int pass)
{
    float sigma = radius / 3.0f;
    float box_width = sqrt(sigma * sigma * 12.0f / pass + 1);
    return box_width;
}


BlurRow::BlurRow(QOpenGLExtraFunctions* gl, int width): m_gl(gl), m_width(width)
{
    std::string s_compute = g_compute;
    std::string defines = "";

    {
        char line[64];
        sprintf(line, "#define THREADS_PER_GROUP %d\n", m_GroupSize);
        defines += line;
    }

    {
        char line[64];
        sprintf(line, "#define SHARED_MEM_SIZE %d\n", m_width);
        defines += line;
    }

    {
        char line[64];
        sprintf(line, "#define DATA_SIZE %d\n", m_width);
        defines += line;
    }

    replace(s_compute, "#DEFINES#", defines.c_str());
    replace(s_compute, "#INPUT_OUTPUT#", g_row_io.c_str());


    GLShader compute_shader(m_gl, GL_COMPUTE_SHADER, s_compute.c_str());
    m_prog = (std::unique_ptr<GLProgram>)(new GLProgram(m_gl, compute_shader));
}

void BlurRow::filter(unsigned tex_id, int height, float filterRadius, float contrast, int numApproxPasses)
{
    float box_width = calcBoxFilterWidth(filterRadius, numApproxPasses);
    float half_box_width = box_width * 0.5f;
    float frac_half_box_width = (half_box_width + 0.5f) - (int)(half_box_width + 0.5f);
    float inv_frac_half_box_width = 1.0f - frac_half_box_width;
    float rcp_box_width = 1.0f / box_width;

    m_gl->glUseProgram(m_prog->m_id);
    m_gl->glUniform1i(0, numApproxPasses - 1);
    m_gl->glUniform1f(1, half_box_width);
    m_gl->glUniform1f(2, frac_half_box_width);
    m_gl->glUniform1f(3, inv_frac_half_box_width);
    m_gl->glUniform1f(4, rcp_box_width);
    m_gl->glUniform1f(5, filterRadius);
    m_gl->glUniform1f(6, contrast);
    m_gl->glBindImageTexture(0, tex_id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);    

    m_gl->glDispatchCompute(height, 1, 1);

    m_gl->glUseProgram(0);

    m_gl->glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

}

BlurCol::BlurCol(QOpenGLExtraFunctions* gl, int height) : m_gl(gl), m_height(height)
{
    std::string s_compute = g_compute;
    std::string defines = "";

    {
        char line[64];
        sprintf(line, "#define THREADS_PER_GROUP %d\n", m_GroupSize);
        defines += line;
    }

    {
        char line[64];
        sprintf(line, "#define SHARED_MEM_SIZE %d\n", m_height);
        defines += line;
    }

    {
        char line[64];
        sprintf(line, "#define DATA_SIZE %d\n", m_height);
        defines += line;
    }

    replace(s_compute, "#DEFINES#", defines.c_str());
    replace(s_compute, "#INPUT_OUTPUT#", g_col_io.c_str());

    GLShader compute_shader(m_gl, GL_COMPUTE_SHADER, s_compute.c_str());
    m_prog = (std::unique_ptr<GLProgram>)(new GLProgram(m_gl, compute_shader));
}


void BlurCol::filter(unsigned tex_id, int weight, float filterRadius, float contrast, int numApproxPasses)
{
    float box_width = calcBoxFilterWidth(filterRadius, numApproxPasses);
    float half_box_width = box_width * 0.5f;
    float frac_half_box_width = (half_box_width + 0.5f) - (int)(half_box_width + 0.5f);
    float inv_frac_half_box_width = 1.0f - frac_half_box_width;
    float rcp_box_width = 1.0f / box_width;

    m_gl->glUseProgram(m_prog->m_id);
    m_gl->glUniform1i(0, numApproxPasses - 1);
    m_gl->glUniform1f(1, half_box_width);
    m_gl->glUniform1f(2, frac_half_box_width);
    m_gl->glUniform1f(3, inv_frac_half_box_width);
    m_gl->glUniform1f(4, rcp_box_width);
    m_gl->glUniform1f(5, filterRadius);
    m_gl->glUniform1f(6, contrast);
    m_gl->glBindImageTexture(0, tex_id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);

    m_gl->glDispatchCompute(weight, 1, 1);

    m_gl->glUseProgram(0);

    m_gl->glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

}