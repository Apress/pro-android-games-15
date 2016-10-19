#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

// JNI
//#include "com_opengl_shader_ViewRenderer.h"

#include <android/log.h>

#define  LOG_TAG    "libgl2"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

int         Shader[2];              // We have a vertex & a fragment shader
int         Program;                // Totalling one program
float       Angle = 0.0;            // Rotation angle of our object
float       Proj[4][4];             // Projection matrix
int         iProj, iModel;          // Our 2 uniforms
int         lastTicks;              // Holds last time Display() was called so we can compute the time difference
int         id;

// because this variable is accessed from two threads, the main thread 
// and the JavaScript method handler thread, we mark it as volatile so the 
// compiler will always reread it from memory when its used. 
volatile float RotationSpeed = 0.001f; // Rotation speed of our object

// another cross-thread variable, used to stop movement when plugin is in card that's minimized
volatile bool Paused = false;

// Vladimir
static int width = 480;
static int height = 320;

void doSetRotationSpeed(double val)
{
    // we'll make the slowest it goes 0.001, and 
    // the fastest 0.01
    double slowest = -0.005;
    double fastest = 0.005;
    double range = fastest - slowest;

    RotationSpeed = (float)(slowest + ((range*val)/100.0f));
	LOGD("Set rotation speed = %2.3f", RotationSpeed);
}

// Standard GL perspective matrix creation
void Persp(float Proj[4][4], const float FOV, const float ZNear, const float ZFar)
{
    const float Delta   = ZFar - ZNear;

    memset(Proj, 0, sizeof(Proj));

    Proj[0][0] = 1.0f / tanf(FOV * 3.1415926535f / 360.0f);
    //Proj[1][1] = Proj[0][0] / ((float)Surface->h / Surface->w);
	Proj[1][1] = Proj[0][0] / ((float)height / width);
	
    Proj[2][2] = -(ZFar + ZNear) / Delta;
    Proj[2][3] = -1.0f;
    Proj[3][2] = -2.0f * ZFar * ZNear / Delta;
}



// Simple function to create a shader
void LoadShader(char *Code, int ID)
{
	LOGD("Load shader %s id=%d", Code, ID);
	
    // Compile the shader code
    glShaderSource  (ID, 1, (const char **)&Code, NULL); 
    glCompileShader (ID);

    // Verify that it worked
    int ShaderStatus;
    glGetShaderiv(ID, GL_COMPILE_STATUS, &ShaderStatus); 

    // Check the compile status
    if (ShaderStatus != GL_TRUE) {
        LOGE("Error: Failed to compile GLSL program\n");
        int Len = 1024;
        char Error[1024];
        glGetShaderInfoLog(ID, 1024, &Len, Error);
        LOGE(Error);
        exit (-1);
    }
}

// Initializes the application data
//int Init(void) 
int Init(int w, int h) 
{
	width = w;
	height = h;
	
	LOGD("Init: w=%d h=%d", width, height);
	
    // Very basic ambient+diffusion model
    const char VertexShader[] = "                   \
        attribute vec3 Position;                    \
        attribute vec3 Normal;                      \
                                                    \
        uniform mat4 Proj;                          \
        uniform mat4 Model;                         \
                                                    \
        varying vec3 NormVec;                       \
        varying vec3 LighVec;                       \
                                                    \
        void main(void)                             \
        {                                           \
            vec4 Pos = Model * vec4(Position, 1.0); \
                                                    \
            gl_Position = Proj * Pos;               \
                                                    \
            NormVec     = (Model * vec4(Normal,0.0)).xyz;     \
            LighVec     = -Pos.xyz;                 \
        }                                           \
    ";

    const char FragmentShaderRed[] = "                                             \
        varying highp vec3 NormVec;                                             \
        varying highp vec3 LighVec;                                             \
                                                                                \
        void main(void)                                                         \
        {                                                                       \
            lowp vec3 Color = vec3(1.0, 0.0, 0.0);                              \
                                                                                \
            mediump vec3 Norm  = normalize(NormVec);                            \
            mediump vec3 Light = normalize(LighVec);                            \
                                                                                \
            mediump float Diffuse = dot(Norm, Light);                           \
                                                                                \
            gl_FragColor = vec4(Color * (max(Diffuse, 0.0) * 0.6 + 0.4), 1.0);  \
        }                                                                       \
    ";
    
    const char FragmentShaderBlue[] = "                                             \
        varying highp vec3 NormVec;                                             \
        varying highp vec3 LighVec;                                             \
                                                                                \
        void main(void)                                                         \
        {                                                                       \
            lowp vec3 Color = vec3(0.0, 0.0, 1.0);                              \
                                                                                \
            mediump vec3 Norm  = normalize(NormVec);                            \
            mediump vec3 Light = normalize(LighVec);                            \
                                                                                \
            mediump float Diffuse = dot(Norm, Light);                           \
                                                                                \
            gl_FragColor = vec4(Color * (max(Diffuse, 0.0) * 0.6 + 0.4), 1.0);  \
        }                                                                       \
    ";

    // Create 2 shader programs
    Shader[0] = glCreateShader(GL_VERTEX_SHADER);
    Shader[1] = glCreateShader(GL_FRAGMENT_SHADER);

    LoadShader((char *)VertexShader, Shader[0]);
    
    if ( id == 2 )
    {
        LoadShader((char *)FragmentShaderBlue, Shader[1]);
    }
    else
    {
        LoadShader((char *)FragmentShaderRed, Shader[1]);
    }

    // Create the prorgam and attach the shaders & attributes
    Program   = glCreateProgram();

    glAttachShader(Program, Shader[0]);
    glAttachShader(Program, Shader[1]);

    glBindAttribLocation(Program, 0, "Position");
    glBindAttribLocation(Program, 1, "Normal");

    // Link
    glLinkProgram(Program);

    // Validate our work thus far
    int ShaderStatus;
    glGetProgramiv(Program, GL_LINK_STATUS, &ShaderStatus); 

    if (ShaderStatus != GL_TRUE) {
        LOGE("Error: Failed to link GLSL program\n");
        int Len = 1024;
        char Error[1024];
        glGetProgramInfoLog(Program, 1024, &Len, Error);
        LOGE(Error);
        exit(-1);
    }

    glValidateProgram(Program);

    glGetProgramiv(Program, GL_VALIDATE_STATUS, &ShaderStatus); 

    if (ShaderStatus != GL_TRUE) {
        LOGE("Error: Failed to validate GLSL program\n");
        exit(-1);
    }

    // Enable the program
    glUseProgram                (Program);
    glEnableVertexAttribArray   (0);
    glEnableVertexAttribArray   (1);

    // Setup the Projection matrix
    Persp(Proj, 70.0f, 0.1f, 200.0f);

    // Retrieve our uniforms
    iProj   = glGetUniformLocation(Program, "Proj");
    iModel  = glGetUniformLocation(Program, "Model");

    // Basic GL setup
    glClearColor    (0.0, 0.0, 0.0, 1.0);
    glEnable        (GL_CULL_FACE);
    glCullFace      (GL_BACK);

    return GL_TRUE;
}

// Main-loop workhorse function for displaying the object
//void Display(void)
void Display(int time)
{
    // Clear the screen
    glClear (GL_COLOR_BUFFER_BIT);

    float Model[4][4];

    memset(Model, 0, sizeof(Model));

    // Setup the Proj so that the object rotates around the Y axis
    // We'll also translate it appropriately to Display
    Model[0][0] = cosf(Angle);
    Model[1][1] = 1.0f;
    Model[2][0] = sinf(Angle);
    Model[0][2] = -sinf(Angle);
    Model[2][2] = cos(Angle);
    Model[3][2] = -1.0f;   
    Model[3][3] = 1.0f;

    // Constantly rotate the object as a function of time
    int ticks = time; //SDL_GetTicks(); // note current time
    int thisTicks = ticks - lastTicks; // note delta time
    if ( thisTicks > 200 ) thisTicks = 200; // throttling
    Angle += ((float)thisTicks) * RotationSpeed; // apply animation
    lastTicks = ticks; // note for next loop

	//LOGD("Display angle=%.2f ticks=%d speed=%.2f", Angle, thisTicks, RotationSpeed);
	
    // Vertex information
    float PtData[][3] = {
        {0.5f, 0.0380823f, 0.028521f},
        {0.182754f, 0.285237f, 0.370816f},
        {0.222318f, -0.2413f, 0.38028f},
        {0.263663f, -0.410832f, -0.118163f},
        {0.249651f, 0.0109279f, -0.435681f},
        {0.199647f, 0.441122f, -0.133476f},
        {-0.249651f, -0.0109279f, 0.435681f},
        {-0.263663f, 0.410832f, 0.118163f},
        {-0.199647f, -0.441122f, 0.133476f},
        {-0.182754f, -0.285237f, -0.370816f},
        {-0.222318f, 0.2413f, -0.38028f},
        {-0.5f, -0.0380823f, -0.028521f},
    };

    // Face information
    unsigned short FaceData[][3] = {
        {0,1,2,},
        {0,2,3,},
        {0,3,4,},
        {0,4,5,},
        {0,5,1,},
        {1,5,7,},
        {1,7,6,},
        {1,6,2,},
        {2,6,8,},
        {2,8,3,},
        {3,8,9,},
        {3,9,4,},
        {4,9,10,},
        {4,10,5,},
        {5,10,7,},
        {6,7,11,},
        {6,11,8,},
        {7,10,11,},
        {8,11,9,},
        {9,11,10,},
    };


    // Draw the icosahedron
    glUseProgram            (Program);
    glUniformMatrix4fv      (iProj, 1, false, (const float *)&Proj[0][0]);
    glUniformMatrix4fv      (iModel, 1, false, (const float *)&Model[0][0]);

    glVertexAttribPointer   (0, 3, GL_FLOAT, 0, 0, &PtData[0][0]);
    glVertexAttribPointer   (1, 3, GL_FLOAT, GL_TRUE, 0, &PtData[0][0]);

    glDrawElements          (GL_TRIANGLES, sizeof(FaceData) / sizeof(unsigned short), 
                            GL_UNSIGNED_SHORT, &FaceData[0][0]);
}

extern "C" {

JNIEXPORT void JNICALL Java_com_opengl_shader_ViewRenderer_initialize
  (JNIEnv * env, jclass cls, jint w, jint h)
{
	LOGD("JNI Initiaize w=%d h=%d", w, h);
	Init(w,h);
}

JNIEXPORT void JNICALL Java_com_opengl_shader_ViewRenderer_drawFrame
  (JNIEnv * env, jclass cls, jint ticks)
{
	//LOGD("JNI DrawFrame ticks=%d", ticks);
	Display(ticks);
}

JNIEXPORT void JNICALL Java_com_opengl_shader_ViewRenderer_setRotationSpeed
  (JNIEnv * env, jclass cls, jint val)
{
	LOGD("Set rotation speed[0-100]=%d", val);
	doSetRotationSpeed((double)val);
}

}