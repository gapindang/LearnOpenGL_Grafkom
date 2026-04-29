// ============================================================
// Tugas Akhir Grafika Komputer
// Visualisasi Komunikasi Data Jaringan Komputer Berbasis 3D
// OpenGL 3.3 Core Profile
// ============================================================
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <learnopengl/shader_s.h>

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

// ─────────────────────────────────────────────────────────────
const unsigned int SCR_WIDTH  = 1280;
const unsigned int SCR_HEIGHT = 720;
const float PI = 3.14159265358979f;

// ─────────────────────────────────────────────────────────────
// 8×8 Bitmap Font (ASCII 32–127)
// Bit ordering: bit0 = LEFTMOST pixel of each row (LSB-first)
// ─────────────────────────────────────────────────────────────
static const uint8_t g_font8x8[96][8] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // 32 ' '
    {0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00}, // 33 !
    {0x6C,0x6C,0x48,0x00,0x00,0x00,0x00,0x00}, // 34 "
    {0x6C,0x6C,0xFE,0x6C,0xFE,0x6C,0x6C,0x00}, // 35 #
    {0x18,0x7E,0x1A,0x7C,0xD0,0x7E,0x18,0x00}, // 36 $
    {0xC2,0x64,0x30,0x18,0x0C,0x46,0x86,0x00}, // 37 %
    {0x1C,0x36,0x1C,0x6E,0x7B,0x33,0x6E,0x00}, // 38 &
    {0x18,0x18,0x0C,0x00,0x00,0x00,0x00,0x00}, // 39 '
    {0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00}, // 40 (
    {0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00}, // 41 )
    {0x00,0x36,0x1C,0x7F,0x1C,0x36,0x00,0x00}, // 42 *
    {0x00,0x0C,0x0C,0x3F,0x0C,0x0C,0x00,0x00}, // 43 +
    {0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,0x06}, // 44 ,
    {0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00}, // 45 -
    {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00}, // 46 .
    {0x60,0x30,0x18,0x0C,0x06,0x03,0x01,0x00}, // 47 /
    {0x3C,0x66,0x76,0x6E,0x66,0x66,0x3C,0x00}, // 48 0
    {0x18,0x1C,0x18,0x18,0x18,0x18,0x7E,0x00}, // 49 1
    {0x3C,0x66,0x60,0x30,0x18,0x0C,0x7E,0x00}, // 50 2
    {0x3C,0x66,0x60,0x38,0x60,0x66,0x3C,0x00}, // 51 3
    {0x70,0x78,0x6C,0x66,0xFF,0x60,0x60,0x00}, // 52 4
    {0x7E,0x06,0x1E,0x60,0x60,0x66,0x3C,0x00}, // 53 5
    {0x38,0x0C,0x06,0x3E,0x66,0x66,0x3C,0x00}, // 54 6
    {0x7E,0x60,0x30,0x18,0x0C,0x0C,0x0C,0x00}, // 55 7
    {0x3C,0x66,0x66,0x3C,0x66,0x66,0x3C,0x00}, // 56 8
    {0x3C,0x66,0x66,0x7C,0x60,0x30,0x1C,0x00}, // 57 9
    {0x00,0x18,0x18,0x00,0x18,0x18,0x00,0x00}, // 58 :
    {0x00,0x18,0x18,0x00,0x18,0x18,0x0C,0x00}, // 59 ;
    {0x30,0x18,0x0C,0x06,0x0C,0x18,0x30,0x00}, // 60 <
    {0x00,0x00,0x7E,0x00,0x7E,0x00,0x00,0x00}, // 61 =
    {0x06,0x0C,0x18,0x30,0x18,0x0C,0x06,0x00}, // 62 >
    {0x3C,0x66,0x60,0x30,0x18,0x00,0x18,0x00}, // 63 ?
    {0x3E,0x63,0x79,0x79,0x79,0x03,0x1E,0x00}, // 64 @
    {0x0C,0x1E,0x33,0x33,0x3F,0x33,0x33,0x00}, // 65 A
    {0x1F,0x33,0x33,0x1F,0x33,0x33,0x1F,0x00}, // 66 B
    {0x3C,0x66,0x06,0x06,0x06,0x66,0x3C,0x00}, // 67 C
    {0x1F,0x33,0x63,0x63,0x63,0x33,0x1F,0x00}, // 68 D
    {0x7F,0x03,0x03,0x1F,0x03,0x03,0x7F,0x00}, // 69 E
    {0x7F,0x03,0x03,0x1F,0x03,0x03,0x03,0x00}, // 70 F
    {0x3C,0x66,0x06,0x76,0x66,0x66,0x7C,0x00}, // 71 G
    {0x33,0x33,0x33,0x3F,0x33,0x33,0x33,0x00}, // 72 H
    {0x1E,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00}, // 73 I
    {0x78,0x30,0x30,0x30,0x33,0x33,0x1E,0x00}, // 74 J
    {0x63,0x33,0x1B,0x0F,0x1B,0x33,0x63,0x00}, // 75 K
    {0x03,0x03,0x03,0x03,0x03,0x03,0x7F,0x00}, // 76 L
    {0x41,0x63,0x77,0x7F,0x6B,0x63,0x63,0x00}, // 77 M
    {0x63,0x67,0x6F,0x7B,0x73,0x63,0x63,0x00}, // 78 N
    {0x1E,0x33,0x63,0x63,0x63,0x33,0x1E,0x00}, // 79 O
    {0x1F,0x33,0x33,0x1F,0x03,0x03,0x03,0x00}, // 80 P
    {0x1E,0x33,0x63,0x63,0x6B,0x1E,0x38,0x00}, // 81 Q
    {0x1F,0x33,0x33,0x1F,0x1B,0x33,0x63,0x00}, // 82 R
    {0x3C,0x66,0x06,0x3C,0x60,0x66,0x3C,0x00}, // 83 S
    {0x3F,0x2D,0x0C,0x0C,0x0C,0x0C,0x1E,0x00}, // 84 T
    {0x33,0x33,0x33,0x33,0x33,0x33,0x3F,0x00}, // 85 U
    {0x33,0x33,0x33,0x33,0x33,0x1E,0x0C,0x00}, // 86 V
    {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00}, // 87 W
    {0x63,0x63,0x36,0x1C,0x1C,0x36,0x63,0x00}, // 88 X
    {0x33,0x33,0x33,0x1E,0x0C,0x0C,0x1E,0x00}, // 89 Y
    {0x7F,0x63,0x31,0x18,0x4C,0x66,0x7F,0x00}, // 90 Z
    {0x1E,0x06,0x06,0x06,0x06,0x06,0x1E,0x00}, // 91 [
    {0x03,0x06,0x0C,0x18,0x30,0x60,0x40,0x00}, // 92 backslash
    {0x1E,0x18,0x18,0x18,0x18,0x18,0x1E,0x00}, // 93 ]
    {0x08,0x1C,0x36,0x63,0x00,0x00,0x00,0x00}, // 94 ^
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF}, // 95 _
    {0x0C,0x0C,0x18,0x00,0x00,0x00,0x00,0x00}, // 96 `
    {0x00,0x00,0x3E,0x60,0x7C,0x66,0x7C,0x00}, // 97 a
    {0x03,0x03,0x1F,0x33,0x33,0x33,0x1F,0x00}, // 98 b
    {0x00,0x00,0x3C,0x06,0x06,0x06,0x3C,0x00}, // 99 c
    {0x30,0x30,0x7C,0x66,0x66,0x66,0x7C,0x00}, // 100 d
    {0x00,0x00,0x3C,0x66,0x7E,0x06,0x3C,0x00}, // 101 e
    {0x38,0x0C,0x0C,0x1E,0x0C,0x0C,0x0C,0x00}, // 102 f
    {0x00,0x00,0x7C,0x66,0x66,0x7C,0x60,0x3C}, // 103 g
    {0x03,0x03,0x1B,0x37,0x33,0x33,0x33,0x00}, // 104 h
    {0x18,0x00,0x1C,0x18,0x18,0x18,0x3C,0x00}, // 105 i
    {0x30,0x00,0x38,0x30,0x30,0x33,0x1E,0x00}, // 106 j
    {0x03,0x03,0x33,0x1B,0x0F,0x1B,0x33,0x00}, // 107 k
    {0x1C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00}, // 108 l
    {0x00,0x00,0x37,0x7F,0x6B,0x63,0x63,0x00}, // 109 m
    {0x00,0x00,0x1F,0x33,0x33,0x33,0x33,0x00}, // 110 n
    {0x00,0x00,0x1E,0x33,0x33,0x33,0x1E,0x00}, // 111 o
    {0x00,0x00,0x1F,0x33,0x33,0x1F,0x03,0x03}, // 112 p
    {0x00,0x00,0x7C,0x66,0x66,0x7C,0x60,0x60}, // 113 q
    {0x00,0x00,0x37,0x0F,0x03,0x03,0x03,0x00}, // 114 r
    {0x00,0x00,0x3C,0x06,0x1C,0x30,0x1E,0x00}, // 115 s
    {0x0C,0x0C,0x3E,0x0C,0x0C,0x0C,0x38,0x00}, // 116 t
    {0x00,0x00,0x33,0x33,0x33,0x33,0x7E,0x00}, // 117 u
    {0x00,0x00,0x33,0x33,0x33,0x1E,0x0C,0x00}, // 118 v
    {0x00,0x00,0x63,0x6B,0x7F,0x3E,0x36,0x00}, // 119 w
    {0x00,0x00,0x63,0x36,0x1C,0x36,0x63,0x00}, // 120 x
    {0x00,0x00,0x33,0x33,0x3E,0x30,0x1C,0x00}, // 121 y
    {0x00,0x00,0x7E,0x30,0x18,0x0C,0x7E,0x00}, // 122 z
    {0x38,0x0C,0x0C,0x07,0x0C,0x0C,0x38,0x00}, // 123 {
    {0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x00}, // 124 |
    {0x07,0x0C,0x0C,0x38,0x0C,0x0C,0x07,0x00}, // 125 }
    {0x6E,0x3B,0x00,0x00,0x00,0x00,0x00,0x00}, // 126 ~
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // 127 DEL
};

// ─────────────────────────────────────────────────────────────
// Build RGBA texture from ASCII text using bitmap font above.
// FIX: use (glyph[row] >> col) & 1  ← bit0 = leftmost pixel
// ─────────────────────────────────────────────────────────────
unsigned int createLabelTexture(const std::string& text,
                                uint8_t r=255,uint8_t g=255,uint8_t b=80)
{
    int n=(int)text.size();
    int W=n*8, H=8;
    std::vector<uint8_t> pix(W*H*4, 0);

    for(int ci=0;ci<n;ci++){
        int c=(unsigned char)text[ci];
        if(c<32||c>127) c=32;
        const uint8_t* glyph=g_font8x8[c-32];
        for(int row=0;row<8;row++){
            for(int col=0;col<8;col++){
                // bit0 = leftmost pixel (col=0)
                bool lit = (glyph[row] >> col) & 1;
                int px = ci*8+col;
                int idx= (row*W+px)*4;
                pix[idx+0]=lit?r:0;
                pix[idx+1]=lit?g:0;
                pix[idx+2]=lit?b:0;
                pix[idx+3]=lit?255:0; // transparent background
            }
        }
    }

    unsigned int id;
    glGenTextures(1,&id);
    glBindTexture(GL_TEXTURE_2D,id);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,W,H,0,GL_RGBA,GL_UNSIGNED_BYTE,pix.data());
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D,0);
    return id;
}

// ─────────────────────────────────────────────────────────────
// Inline billboard shader
// ─────────────────────────────────────────────────────────────
static const char* kBVS=R"glsl(
#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec2 aUV;
out vec2 vUV;
uniform mat4 VP;
void main(){ gl_Position=VP*vec4(aPos,1.0); vUV=aUV; }
)glsl";
static const char* kBFS=R"glsl(
#version 330 core
in vec2 vUV;
out vec4 FragColor;
uniform sampler2D uTex;
void main(){
    vec4 c=texture(uTex,vUV);
    if(c.a<0.1) discard;
    FragColor=c;
}
)glsl";

unsigned int compileBillboard(){
    auto mk=[](GLenum t,const char* s)->unsigned int{
        unsigned int sh=glCreateShader(t);
        glShaderSource(sh,1,&s,NULL);glCompileShader(sh);return sh;};
    unsigned int vs=mk(GL_VERTEX_SHADER,kBVS);
    unsigned int fs=mk(GL_FRAGMENT_SHADER,kBFS);
    unsigned int p=glCreateProgram();
    glAttachShader(p,vs);glAttachShader(p,fs);glLinkProgram(p);
    glDeleteShader(vs);glDeleteShader(fs);return p;
}

// ─────────────────────────────────────────────────────────────
// renderBillboard — always faces camera
//
// UV convention (to display text left-to-right):
//   BL=(0,1), BR=(1,1), TR=(1,0), TL=(0,0)
//   UV.x increases LEFT→RIGHT (screen space, via camera right vector)
//   UV.y=0=top of texture (font row 0 = top of character)
// ─────────────────────────────────────────────────────────────
void renderBillboard(unsigned int prog, unsigned int tex,
                     const glm::vec3& worldPos, float width, float height,
                     const glm::mat4& view, const glm::mat4& proj)
{
    // Extract world-space camera right and up from the view matrix rows
    glm::vec3 cr(view[0][0], view[1][0], view[2][0]); // world right
    glm::vec3 cu(view[0][1], view[1][1], view[2][1]); // world up

    float hw=width*0.5f, hh=height*0.5f;
    glm::vec3 v0=worldPos-cr*hw-cu*hh; // BL
    glm::vec3 v1=worldPos+cr*hw-cu*hh; // BR
    glm::vec3 v2=worldPos+cr*hw+cu*hh; // TR
    glm::vec3 v3=worldPos-cr*hw+cu*hh; // TL

    // UV: BL=(0,1), BR=(1,1), TR=(1,0), TL=(0,0)
    // Texture row-0 (font top) → UV.y=0 → top of quad ✓
    // Texture col-0 (font left) → UV.x=0 → screen left ✓
    float verts[]={
        v0.x,v0.y,v0.z, 0.f,1.f,
        v1.x,v1.y,v1.z, 1.f,1.f,
        v2.x,v2.y,v2.z, 1.f,0.f,
        v3.x,v3.y,v3.z, 0.f,0.f,
    };
    unsigned int idx[]={0,1,2,0,2,3};

    unsigned int vao,vbo,ebo;
    glGenVertexArrays(1,&vao);
    glGenBuffers(1,&vbo);
    glGenBuffers(1,&ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(verts),verts,GL_STREAM_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(idx),idx,GL_STREAM_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(prog);
    glm::mat4 VP=proj*view;
    glUniformMatrix4fv(glGetUniformLocation(prog,"VP"),1,GL_FALSE,glm::value_ptr(VP));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,tex);
    glUniform1i(glGetUniformLocation(prog,"uTex"),0);
    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);

    glBindVertexArray(0);
    glDeleteBuffers(1,&vbo);
    glDeleteBuffers(1,&ebo);
    glDeleteVertexArrays(1,&vao);
}

// ─────────────────────────────────────────────────────────────
// App state
// ─────────────────────────────────────────────────────────────
enum AppState { MENU, LOCAL_MODE, GLOBAL_MODE };
AppState appState = MENU;

// ─────────────────────────────────────────────────────────────
// Camera
// ─────────────────────────────────────────────────────────────
struct Camera {
    glm::vec3 position,front,up;
    float yaw,pitch,fov,speed,sensitivity;
    Camera():position(0,2,8),front(0,0,-1),up(0,1,0),
             yaw(-90),pitch(0),fov(45),speed(4),sensitivity(0.08f){}
    glm::mat4 getView() const{ return glm::lookAt(position,position+front,up); }
    void updateVectors(){
        glm::vec3 f;
        f.x=cosf(glm::radians(yaw))*cosf(glm::radians(pitch));
        f.y=sinf(glm::radians(pitch));
        f.z=sinf(glm::radians(yaw))*cosf(glm::radians(pitch));
        front=glm::normalize(f);
    }
    void resetLocal(){
        // Wide angle looking slightly down to see whole scene
        position={0.f,4.5f,16.f};
        yaw=-90;pitch=-16;fov=55;
        updateVectors();
    }
    void resetGlobal(){
        position={0.f,2.5f,8.5f};
        yaw=-90;pitch=-16;fov=48;
        updateVectors();
    }
};
Camera camera;
bool firstMouse=true,mouseButtonDown=false;
float lastX=SCR_WIDTH/2.f,lastY=SCR_HEIGHT/2.f;

// ─────────────────────────────────────────────────────────────
// Packet animation
// ─────────────────────────────────────────────────────────────
enum PacketState { IDLE, ANIMATING, DELIVERED };
PacketState packetState = IDLE;

std::vector<glm::vec3> localWaypoints;
float packetT=0.f;
int   packetSeg=0;
int   activeDevice=0; // 1=PCA,2=SW,3=RT,4=PCB

const float LOCAL_SPEED  = 1.5f; // segments / second
const float GLOBAL_SPEED = 0.18f;

// ─────────────────────────────────────────────────────────────
// LOCAL SCENE LAYOUT
//
//           [RT]   ← mounted on rack panel, y=0.9
//           |      ← short vertical cable
//  [PCA]───[SW]──────────────[PCB]
//
// All devices along X-axis. SW and RT form a mini rack unit.
// ─────────────────────────────────────────────────────────────
const glm::vec3 pos_PCA = {-6.0f, 0.0f, 0.0f};
const glm::vec3 pos_SW  = { 0.0f, 0.0f, 0.0f};
const glm::vec3 pos_RT  = { 0.0f, 0.9f, 0.0f}; // on rack, not flying
const glm::vec3 pos_PCB = { 6.0f, 0.0f, 0.0f};

// Packet waypoints: PCA → SW → RT → SW → PCB
// Short waypoints at port positions for smooth cable-following path
void initLocalWaypoints(){
    localWaypoints.clear();
    localWaypoints = {
        pos_PCA  + glm::vec3( 0.45f, 0.35f, 0),  // PCA top-right port
        pos_SW   + glm::vec3(-0.85f, 0.0f,  0),  // SW left port
        pos_SW   + glm::vec3( 0.0f,  0.11f, 0),  // SW top port
        pos_RT   + glm::vec3( 0.0f, -0.25f, 0),  // RT bottom port
        pos_RT,                                    // RT center
        pos_SW   + glm::vec3( 0.0f,  0.11f, 0),  // SW top port (return)
        pos_SW   + glm::vec3( 0.85f, 0.0f,  0),  // SW right port
        pos_PCB  + glm::vec3(-0.45f, 0.35f, 0),  // PCB top-left port
    };
}

glm::vec3 getLocalPacketPos(){
    int mx=(int)localWaypoints.size()-1;
    if(packetSeg>=mx||localWaypoints.empty()) return pos_PCA;
    return glm::mix(localWaypoints[packetSeg], localWaypoints[packetSeg+1], packetT);
}

// Segment → device highlight
int segToDevice(int s){
    if(s<=1) return 1; // PCA
    if(s<=3) return 2; // SW
    if(s==4) return 3; // RT
    if(s<=6) return 2; // SW return
    return 4;           // PCB
}

// ─────────────────────────────────────────────────────────────
// GLOBAL SCENE
// Buildings attached to earth surface via sphere-surface normals.
// Both normals point toward the camera (+Z front hemisphere).
// ─────────────────────────────────────────────────────────────
// Building surface normals in local earth frame.
// High Z (toward camera) ensures buildings appear on the FRONT
// hemisphere and are not occluded by the earth's own depth facets.
const glm::vec3 nA_base = glm::normalize(glm::vec3(-0.42f, 0.12f, 0.90f)); // Asia
const glm::vec3 nB_base = glm::normalize(glm::vec3( 0.42f, 0.12f, 0.90f)); // America
float earthRotDeg=0.f;

glm::vec3 rotateY(const glm::vec3& v, float deg){
    float r=glm::radians(deg),c=cosf(r),s=sinf(r);
    return {v.x*c+v.z*s, v.y, -v.x*s+v.z*c};
}

// Spherical linear interpolation between unit vectors
glm::vec3 slerpVec(const glm::vec3& a, const glm::vec3& b, float t){
    float d=glm::clamp(glm::dot(a,b),-1.f,1.f);
    float theta=acosf(d);
    if(theta<1e-5f) return glm::normalize(glm::mix(a,b,t));
    glm::vec3 rel=glm::normalize(b-a*d);
    return a*cosf(theta*t)+rel*sinf(theta*t);
}

// Rotation matrix: local +Y aligns with world normal n
glm::mat4 alignYToNormal(const glm::vec3& n){
    glm::vec3 worldUp={0,1,0};
    glm::vec3 right;
    if(fabsf(glm::dot(n,worldUp))<0.99f)
        right=glm::normalize(glm::cross(worldUp,n));
    else
        right={1,0,0};
    glm::vec3 fwd=glm::normalize(glm::cross(n,right));
    glm::mat4 R(1.f);
    R[0]=glm::vec4(right,0);
    R[1]=glm::vec4(n,    0);
    R[2]=glm::vec4(fwd,  0);
    return R;
}

// ─────────────────────────────────────────────────────────────
// Highlight helper
// ─────────────────────────────────────────────────────────────
float hlFactor(int id){
    if(appState==LOCAL_MODE){
        if(packetState==IDLE) return 0.f;
        return (activeDevice==id) ? 0.55f : 0.f;
    }
    if(appState==GLOBAL_MODE){
        if(id==1&&packetState==ANIMATING) return 0.40f;
        if(id==2&&packetState==DELIVERED) return 0.55f;
    }
    return 0.f;
}

// ─────────────────────────────────────────────────────────────
// Geometry builders
// ─────────────────────────────────────────────────────────────
std::vector<float> buildCube(){
    return {
        // back
        -0.5f,-0.5f,-0.5f, 0,0,-1, 0,0, 0.5f,-0.5f,-0.5f, 0,0,-1, 1,0,
         0.5f, 0.5f,-0.5f, 0,0,-1, 1,1, 0.5f, 0.5f,-0.5f, 0,0,-1, 1,1,
        -0.5f, 0.5f,-0.5f, 0,0,-1, 0,1,-0.5f,-0.5f,-0.5f, 0,0,-1, 0,0,
        // front
        -0.5f,-0.5f, 0.5f, 0,0,1, 0,0, 0.5f,-0.5f, 0.5f, 0,0,1, 1,0,
         0.5f, 0.5f, 0.5f, 0,0,1, 1,1, 0.5f, 0.5f, 0.5f, 0,0,1, 1,1,
        -0.5f, 0.5f, 0.5f, 0,0,1, 0,1,-0.5f,-0.5f, 0.5f, 0,0,1, 0,0,
        // left
        -0.5f, 0.5f, 0.5f,-1,0,0, 1,0,-0.5f, 0.5f,-0.5f,-1,0,0, 1,1,
        -0.5f,-0.5f,-0.5f,-1,0,0, 0,1,-0.5f,-0.5f,-0.5f,-1,0,0, 0,1,
        -0.5f,-0.5f, 0.5f,-1,0,0, 0,0,-0.5f, 0.5f, 0.5f,-1,0,0, 1,0,
        // right
         0.5f, 0.5f, 0.5f, 1,0,0, 1,0, 0.5f, 0.5f,-0.5f, 1,0,0, 1,1,
         0.5f,-0.5f,-0.5f, 1,0,0, 0,1, 0.5f,-0.5f,-0.5f, 1,0,0, 0,1,
         0.5f,-0.5f, 0.5f, 1,0,0, 0,0, 0.5f, 0.5f, 0.5f, 1,0,0, 1,0,
        // bottom
        -0.5f,-0.5f,-0.5f, 0,-1,0, 0,1, 0.5f,-0.5f,-0.5f, 0,-1,0, 1,1,
         0.5f,-0.5f, 0.5f, 0,-1,0, 1,0, 0.5f,-0.5f, 0.5f, 0,-1,0, 1,0,
        -0.5f,-0.5f, 0.5f, 0,-1,0, 0,0,-0.5f,-0.5f,-0.5f, 0,-1,0, 0,1,
        // top
        -0.5f, 0.5f,-0.5f, 0,1,0, 0,1, 0.5f, 0.5f,-0.5f, 0,1,0, 1,1,
         0.5f, 0.5f, 0.5f, 0,1,0, 1,0, 0.5f, 0.5f, 0.5f, 0,1,0, 1,0,
        -0.5f, 0.5f, 0.5f, 0,1,0, 0,0,-0.5f, 0.5f,-0.5f, 0,1,0, 0,1,
    };
}

struct SphereMesh{ std::vector<float> v; std::vector<unsigned int> i; };
SphereMesh buildSphere(int st=28,int sl=40){
    SphereMesh m;
    for(int i=0;i<=st;i++){
        float phi=PI*(float)i/st;
        for(int j=0;j<=sl;j++){
            float th=2.f*PI*(float)j/sl;
            float x=sinf(phi)*cosf(th),y=cosf(phi),z=sinf(phi)*sinf(th);
            m.v.insert(m.v.end(),{x,y,z,x,y,z,(float)j/sl,1.f-(float)i/st});
        }
    }
    for(int i=0;i<st;i++)
        for(int j=0;j<sl;j++){
            unsigned a=(unsigned)(i*(sl+1)+j),b=a+(unsigned)(sl+1);
            m.i.insert(m.i.end(),{a,b,a+1,b,b+1,a+1});
        }
    return m;
}

// ─────────────────────────────────────────────────────────────
// GPU resources
// ─────────────────────────────────────────────────────────────
unsigned int cubeVAO=0;
unsigned int sphereVAO=0, sphereVBO=0, sphereEBO_id=0;
int          sphereIdxCnt=0;

unsigned int lineShader=0, billboardProg=0;
// Labels: 0=PCA, 1=SW, 2=RT, 3=PCB, 4=ASIA, 5=AMERICA
unsigned int texLabel[6]={};
unsigned int texEarth=0, texContainer=0;

// ─────────────────────────────────────────────────────────────
// VAO creation
// ─────────────────────────────────────────────────────────────
unsigned int createVAO8f(const std::vector<float>& v){
    unsigned int VAO,VBO;
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,v.size()*sizeof(float),v.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*4,(void*)0);              glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*4,(void*)(3*4));          glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*4,(void*)(6*4));          glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    return VAO;
}

// ─────────────────────────────────────────────────────────────
// Line shader
// ─────────────────────────────────────────────────────────────
static const char* kLVS=R"(#version 330 core
layout(location=0) in vec3 aPos; uniform mat4 MVP;
void main(){gl_Position=MVP*vec4(aPos,1.0);})";
static const char* kLFS=R"(#version 330 core
out vec4 FragColor; uniform vec3 col;
void main(){FragColor=vec4(col,1.0);})";

unsigned int compileLineShader(){
    auto mk=[](GLenum t,const char* s)->unsigned int{
        unsigned int sh=glCreateShader(t);
        glShaderSource(sh,1,&s,NULL);glCompileShader(sh);return sh;};
    unsigned int p=glCreateProgram();
    auto vs=mk(GL_VERTEX_SHADER,kLVS),fs=mk(GL_FRAGMENT_SHADER,kLFS);
    glAttachShader(p,vs);glAttachShader(p,fs);glLinkProgram(p);
    glDeleteShader(vs);glDeleteShader(fs);return p;
}

void drawLineStrip(const std::vector<glm::vec3>& pts, const glm::vec3& c,
                   const glm::mat4& V, const glm::mat4& P, float w=2.5f)
{
    std::vector<float> flat;
    flat.reserve(pts.size()*3);
    for(auto& p:pts){flat.push_back(p.x);flat.push_back(p.y);flat.push_back(p.z);}
    unsigned int vao,vbo;
    glGenVertexArrays(1,&vao);glGenBuffers(1,&vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,flat.size()*4,flat.data(),GL_STREAM_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,12,(void*)0);glEnableVertexAttribArray(0);
    glUseProgram(lineShader);
    glm::mat4 MVP=P*V;
    glUniformMatrix4fv(glGetUniformLocation(lineShader,"MVP"),1,GL_FALSE,glm::value_ptr(MVP));
    glUniform3fv(glGetUniformLocation(lineShader,"col"),1,glm::value_ptr(c));
    glLineWidth(w);
    glDrawArrays(GL_LINE_STRIP,0,(int)pts.size());
    glBindVertexArray(0);
    glDeleteBuffers(1,&vbo);glDeleteVertexArrays(1,&vao);
}

void drawLine(glm::vec3 a, glm::vec3 b, glm::vec3 c,
              const glm::mat4& V, const glm::mat4& P, float w=2.5f){
    drawLineStrip({a,b},c,V,P,w);
}

// Geodesic arc on sphere surface via SLERP
void drawGeoArc(const glm::vec3& nA, const glm::vec3& nB, float R,
                const glm::vec3& c, const glm::mat4& V, const glm::mat4& P,int seg=80){
    std::vector<glm::vec3> pts;
    for(int i=0;i<=seg;i++) pts.push_back(slerpVec(nA,nB,(float)i/seg)*R);
    drawLineStrip(pts,c,V,P,3.f);
}

// ─────────────────────────────────────────────────────────────
// Texture loader
// ─────────────────────────────────────────────────────────────
unsigned int loadTex(const char* path){
    unsigned int id; glGenTextures(1,&id);
    stbi_set_flip_vertically_on_load(true);
    int w,h,ch; unsigned char* d=stbi_load(path,&w,&h,&ch,0);
    if(d){
        GLenum fmt=(ch==4)?GL_RGBA:(ch==1?GL_RED:GL_RGB);
        glBindTexture(GL_TEXTURE_2D,id);
        glTexImage2D(GL_TEXTURE_2D,0,fmt,w,h,0,fmt,GL_UNSIGNED_BYTE,d);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        std::cout<<"[Tex OK] "<<path<<"\n";
    }else{
        unsigned char white[4]={180,180,180,255};
        glBindTexture(GL_TEXTURE_2D,id);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,1,1,0,GL_RGBA,GL_UNSIGNED_BYTE,white);
        std::cout<<"[Tex MISS] "<<path<<"\n";
    }
    stbi_image_free(d);
    glBindTexture(GL_TEXTURE_2D,0);
    return id;
}

// ─────────────────────────────────────────────────────────────
// Draw primitives via Phong shader
// ─────────────────────────────────────────────────────────────
void drawMesh(Shader& sh, unsigned int VAO, int n,
              const glm::mat4& M, const glm::vec3& col,
              float hlMix, const glm::vec3& hlCol,
              unsigned int texID=0, float useTex=0.f)
{
    sh.setMat4("model",M);
    sh.setVec3("objectColor",col);
    sh.setFloat("highlightMix",hlMix);
    sh.setVec3("highlightColor",hlCol);
    sh.setFloat("useTexture",useTex);
    if(useTex>0.5f){glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D,texID);}
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES,0,n);
    glBindVertexArray(0);
}

void drawSphere(Shader& sh, const glm::mat4& M, const glm::vec3& col,
                float hlMix, const glm::vec3& hlCol,
                unsigned int texID=0, float useTex=0.f)
{
    if(!sphereVAO||!sphereIdxCnt) return; // safety guard
    sh.setMat4("model",M);
    sh.setVec3("objectColor",col);
    sh.setFloat("highlightMix",hlMix);
    sh.setVec3("highlightColor",hlCol);
    sh.setFloat("useTexture",useTex);
    if(useTex>0.5f){glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D,texID);}
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES,sphereIdxCnt,GL_UNSIGNED_INT,0);
    glBindVertexArray(0);
}

// ─────────────────────────────────────────────────────────────
// Status line
// ─────────────────────────────────────────────────────────────
void printStatus(const std::string& s){
    static std::string last;
    if(s!=last){std::cout<<"\r[STATUS] "<<s<<"          "<<std::flush;last=s;}
}

// ─────────────────────────────────────────────────────────────
// Draw LOCAL Scene
// ─────────────────────────────────────────────────────────────
void drawLocalScene(Shader& sh, const glm::mat4& V, const glm::mat4& P){
    sh.use();
    sh.setMat4("view",V);
    sh.setMat4("projection",P);
    sh.setVec3("lightPos", {2.f,10.f,8.f});
    sh.setVec3("viewPos",  camera.position);
    sh.setVec3("lightColor",{1.f,1.f,1.f});
    sh.setInt("texture1",0);

    const glm::vec3 hlY={1.f,0.95f,0.15f};
    const glm::vec3 hlB={0.2f,0.8f,1.f};

    // ── Ground ──
    {
        glm::mat4 m=glm::translate(glm::mat4(1),{0,-0.28f,0});
        m=glm::scale(m,{22.f,0.04f,5.f});
        drawMesh(sh,cubeVAO,36,m,{0.08f,0.10f,0.12f},0,glm::vec3(0));
    }

    // ── Rack Panel (behind SW and RT) ──
    {
        glm::mat4 m=glm::translate(glm::mat4(1),{0.f,0.45f,-0.42f});
        m=glm::scale(m,{2.2f,1.55f,0.08f});
        drawMesh(sh,cubeVAO,36,m,{0.12f,0.13f,0.15f},0,glm::vec3(0));
    }

    // ── PC-A ──
    {
        float hl=hlFactor(1);
        // Monitor
        glm::mat4 mb=glm::translate(glm::mat4(1),pos_PCA);
        mb=glm::scale(mb,{0.90f,0.70f,0.50f});
        drawMesh(sh,cubeVAO,36,mb,{0.35f,0.37f,0.40f},hl,hlY);
        // Screen
        glm::mat4 ms=glm::translate(glm::mat4(1),pos_PCA+glm::vec3(0,0,-0.26f));
        ms=glm::scale(ms,{0.70f,0.50f,0.02f});
        drawMesh(sh,cubeVAO,36,ms,{0.05f,0.28f,0.52f},hl*0.5f,hlY);
        // Stand
        glm::mat4 mst=glm::translate(glm::mat4(1),pos_PCA+glm::vec3(0,-0.50f,0));
        mst=glm::scale(mst,{0.10f,0.30f,0.10f});
        drawMesh(sh,cubeVAO,36,mst,{0.28f,0.28f,0.28f},0,glm::vec3(0));
    }

    // ── Switch ──
    {
        float hl=hlFactor(2);
        glm::mat4 m=glm::translate(glm::mat4(1),pos_SW+glm::vec3(0,-0.07f,0));
        m=glm::scale(m,{1.70f,0.22f,0.60f});
        drawMesh(sh,cubeVAO,36,m,{0.08f,0.46f,0.20f},hl,hlY);
        // LED strip on front
        glm::mat4 ml=glm::translate(glm::mat4(1),pos_SW+glm::vec3(0,0.04f,0.31f));
        ml=glm::scale(ml,{1.40f,0.04f,0.02f});
        drawMesh(sh,cubeVAO,36,ml,{1.f,0.55f,0.f},hl,hlY);
    }

    // ── Router (on rack panel, pos_RT = y=0.9) ──
    {
        float hl=hlFactor(3);
        glm::mat4 m=glm::translate(glm::mat4(1),pos_RT);
        m=glm::scale(m,{1.60f,0.36f,0.55f});
        drawMesh(sh,cubeVAO,36,m,{0.60f,0.12f,0.12f},hl,hlY);
        // Antennas on top
        auto ant=[&](float dx,float tilt){
            glm::mat4 a=glm::translate(glm::mat4(1),pos_RT+glm::vec3(dx,0.38f,0));
            a=glm::rotate(a,glm::radians(tilt),{0,0,1});
            a=glm::scale(a,{0.045f,0.42f,0.045f});
            drawMesh(sh,cubeVAO,36,a,{0.5f,0.5f,0.5f},0,glm::vec3(0));
        };
        ant(-0.35f,-12.f); ant(0.35f,12.f);
        // Front LED
        glm::mat4 ml=glm::translate(glm::mat4(1),pos_RT+glm::vec3(0,-0.02f,0.28f));
        ml=glm::scale(ml,{1.40f,0.04f,0.02f});
        drawMesh(sh,cubeVAO,36,ml,{0.f,0.9f,0.3f},hl,hlY);
    }

    // ── PC-B ──
    {
        float hl=hlFactor(4);
        glm::vec3 hc=(packetState==DELIVERED)?hlB:hlY;
        glm::mat4 mb=glm::translate(glm::mat4(1),pos_PCB);
        mb=glm::scale(mb,{0.90f,0.70f,0.50f});
        drawMesh(sh,cubeVAO,36,mb,{0.35f,0.37f,0.40f},hl,hc);
        glm::mat4 ms=glm::translate(glm::mat4(1),pos_PCB+glm::vec3(0,0,-0.26f));
        ms=glm::scale(ms,{0.70f,0.50f,0.02f});
        drawMesh(sh,cubeVAO,36,ms,{0.05f,0.28f,0.52f},hl*0.5f,hc);
        glm::mat4 mst=glm::translate(glm::mat4(1),pos_PCB+glm::vec3(0,-0.50f,0));
        mst=glm::scale(mst,{0.10f,0.30f,0.10f});
        drawMesh(sh,cubeVAO,36,mst,{0.28f,0.28f,0.28f},0,glm::vec3(0));
    }

    // ── Cables ──
    glm::vec3 cCol={0.55f,0.62f,0.68f};
    // PCA→SW horizontal
    drawLine(localWaypoints[0], localWaypoints[1], cCol,V,P);
    // SW→RT vertical cable (going up to router)
    drawLine(localWaypoints[2], localWaypoints[3], cCol,V,P);
    // SW→PCB horizontal
    drawLine(localWaypoints[6], localWaypoints[7], cCol,V,P);

    // ── Packet Sphere ──
    // Only render if ANIMATING or DELIVERED - never in IDLE state
    if(packetState==ANIMATING){
        glm::vec3 pp=getLocalPacketPos();
        glm::mat4 pm=glm::translate(glm::mat4(1),pp);
        pm=glm::scale(pm,glm::vec3(0.18f));
        drawSphere(sh,pm,{1.f,0.85f,0.f},0.f,glm::vec3(0));
    } else if(packetState==DELIVERED){
        glm::mat4 pm=glm::translate(glm::mat4(1),localWaypoints.back());
        pm=glm::scale(pm,glm::vec3(0.18f));
        drawSphere(sh,pm,{0.2f,1.f,0.35f},0.f,glm::vec3(0));
    }
    // NOTE: packetState==IDLE → nothing drawn
}

// ─────────────────────────────────────────────────────────────
// Draw GLOBAL Scene
// FIX: buildings rotate WITH earth; use stacked uniform-scale
// floors so the normal matrix stays correct (no dark/invisible
// faces from non-uniform world-space scaling).
// ─────────────────────────────────────────────────────────────
void drawGlobalScene(Shader& sh, const glm::mat4& V, const glm::mat4& P){
    sh.use();
    sh.setMat4("view",V);
    sh.setMat4("projection",P);
    sh.setVec3("lightPos",{5.f,5.f,8.f});
    sh.setVec3("viewPos", camera.position);
    sh.setVec3("lightColor",{1.f,1.f,1.f});
    sh.setInt("texture1",0);

    const glm::vec3 hlY  = {1.f,0.75f,0.1f};
    const float earthR   = 2.0f;
    const float cableR   = 2.06f;  // arc just above earth
    // Building params:
    //   baseOff = how far above earth surface the building BASE sits.
    //   Buildings at n*earthR are occluded by the earth's own depth buffer
    //   because the earth front-face (~z=+2) is closer to camera than
    //   n*earthR at oblique angles. Raising by baseOff fixes this.
    const float baseOff  = 0.60f;  // units above earth surface
    const float floorSz  = 0.32f;  // UNIFORM floor cube edge length
    const float floorGap = 0.02f;

    // ── Normals rotate WITH earth ──
    glm::vec3 nA = rotateY(nA_base, earthRotDeg);
    glm::vec3 nB = rotateY(nB_base, earthRotDeg);

    // ── Earth sphere (drawn FIRST, texture rotates) ──
    {
        glm::mat4 m = glm::rotate(glm::mat4(1),glm::radians(earthRotDeg),{0,1,0});
        m = glm::scale(m, glm::vec3(earthR));
        float useTex = texEarth ? 1.f : 0.f;
        drawSphere(sh, m, {0.12f,0.45f,0.80f}, 0.f, glm::vec3(0), texEarth, useTex);
    }

    // ── Geodesic cable arc ──
    drawGeoArc(nA, nB, cableR, {0.35f,0.68f,1.f}, V, P);

    // Helper: draw one multi-floor building raised above earth surface.
    // ─────────────────────────────────────────────────────────────
    // Key design: every cube uses glm::vec3(uniform) scale so that
    // mat3(transpose(inverse(model))) = R (rotation only), giving
    // correct Phong normals. No non-uniform world-space shear.
    auto drawBuilding = [&](const glm::vec3& n, float hl, const glm::vec3& hlCol)
    {
        glm::mat4 Rot  = alignYToNormal(n);

        // Base pivot raised ABOVE earth surface (avoids depth occlusion)
        glm::mat4 Base = glm::translate(glm::mat4(1), n * (earthR + baseOff)) * Rot;

        // Thin support pole from earth surface to building base
        // (visual connection to the sphere surface)
        drawLine(n * earthR, n * (earthR + baseOff),
                 {0.60f,0.62f,0.65f}, V, P, 1.8f);

        // ── 3 floors stacked outward along local +Y ──
        const glm::vec3 fCol[3] = {
            {0.62f, 0.51f, 0.35f},
            {0.56f, 0.46f, 0.30f},
            {0.48f, 0.39f, 0.26f},
        };
        for(int fi = 0; fi < 3; fi++){
            float yc = fi * (floorSz + floorGap) + floorSz * 0.5f;
            // T_surface * R_orient * T_local_Y * S_uniform
            glm::mat4 Mf = Base
                * glm::translate(glm::mat4(1), {0.f, yc, 0.f})
                * glm::scale(glm::mat4(1), glm::vec3(floorSz)); // UNIFORM
            drawMesh(sh, cubeVAO, 36, Mf, fCol[fi], hl, hlCol);
        }

        // Roof slab (slightly wider)
        float roofY = 3.f*(floorSz + floorGap) + 0.02f;
        glm::mat4 Mr = Base
            * glm::translate(glm::mat4(1), {0.f, roofY, 0.f})
            * glm::scale(glm::mat4(1), glm::vec3(floorSz + 0.06f, 0.04f, floorSz + 0.06f));
        drawMesh(sh, cubeVAO, 36, Mr, {0.46f,0.16f,0.10f}, hl, hlCol);

        // Antenna
        float antY = roofY + 0.04f + 0.14f;
        glm::mat4 Ma = Base
            * glm::translate(glm::mat4(1), {0.f, antY, 0.f})
            * glm::scale(glm::mat4(1), glm::vec3(0.030f, 0.28f, 0.030f));
        drawMesh(sh, cubeVAO, 36, Ma, {0.7f,0.7f,0.7f}, 0.f, glm::vec3(0));
    };

    // ── Buildings (drawn AFTER earth, they're outside the sphere) ──
    drawBuilding(nA, hlFactor(1), hlY);
    {
        glm::vec3 hc = (packetState==DELIVERED) ? glm::vec3(0.2f,1.f,0.4f) : hlY;
        drawBuilding(nB, hlFactor(2), hc);
    }

    // ── Packet on geodesic arc ──
    if(packetState==ANIMATING){
        glm::vec3 pn = slerpVec(nA, nB, packetT);
        glm::vec3 pp = pn * (cableR + 0.14f);
        glm::mat4 pm = glm::translate(glm::mat4(1), pp);
        pm = glm::scale(pm, glm::vec3(0.15f));
        drawSphere(sh, pm, {1.f,0.88f,0.f}, 0.f, glm::vec3(0));
    } else if(packetState==DELIVERED){
        glm::vec3 pp = nB * (cableR + 0.14f);
        glm::mat4 pm = glm::translate(glm::mat4(1), pp);
        pm = glm::scale(pm, glm::vec3(0.15f));
        drawSphere(sh, pm, {0.2f,1.f,0.4f}, 0.f, glm::vec3(0));
    }
}

// ─────────────────────────────────────────────────────────────
// Callbacks forward declarations
// ─────────────────────────────────────────────────────────────
void framebuffer_size_callback(GLFWwindow*,int,int);
void mouse_callback(GLFWwindow*,double,double);
void scroll_callback(GLFWwindow*,double,double);
void mouseButton_callback(GLFWwindow*,int,int,int);
void key_callback(GLFWwindow*,int,int,int,int);
void processMovement(GLFWwindow*,float);

// ─────────────────────────────────────────────────────────────
// MAIN
// ─────────────────────────────────────────────────────────────
int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* win=glfwCreateWindow(SCR_WIDTH,SCR_HEIGHT,
        "Tugas Akhir Grafkom - Visualisasi Jaringan 3D",NULL,NULL);
    if(!win){glfwTerminate();return -1;}
    glfwMakeContextCurrent(win);
    glfwSetFramebufferSizeCallback(win,framebuffer_size_callback);
    glfwSetCursorPosCallback(win,mouse_callback);
    glfwSetScrollCallback(win,scroll_callback);
    glfwSetMouseButtonCallback(win,mouseButton_callback);
    glfwSetKeyCallback(win,key_callback);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){return -1;}

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    std::cout<<"\n=== Visualisasi Jaringan 3D ===\n";
    std::cout<<" 1 → Mode Lokal   2 → Mode Global\n";
    std::cout<<" SPACE → Kirim Paket\n";
    std::cout<<" WASD + Klik-Kanan drag → Kamera\n";
    std::cout<<" Scroll → Zoom   R → Reset   ESC → Keluar\n\n";

    // ── Shaders ──
    Shader mainShader("src/tugas_akhir/ta.vs","src/tugas_akhir/ta.fs");
    lineShader    = compileLineShader();
    billboardProg = compileBillboard();

    // ── Cube geometry ──
    cubeVAO = createVAO8f(buildCube());

    // ── Sphere geometry ──
    {
        auto sph = buildSphere(28,40);
        sphereIdxCnt = (int)sph.i.size();
        glGenVertexArrays(1,&sphereVAO);
        glGenBuffers(1,&sphereVBO);
        glGenBuffers(1,&sphereEBO_id);
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER,sphereVBO);
        glBufferData(GL_ARRAY_BUFFER,sph.v.size()*sizeof(float),sph.v.data(),GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,sphereEBO_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,sph.i.size()*sizeof(unsigned int),sph.i.data(),GL_STATIC_DRAW);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,32,(void*)0);   glEnableVertexAttribArray(0);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,32,(void*)12);  glEnableVertexAttribArray(1);
        glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,32,(void*)24);  glEnableVertexAttribArray(2);
        glBindVertexArray(0);
        std::cout<<"[Sphere] VAO="<<sphereVAO<<" idx="<<sphereIdxCnt<<"\n";
    }

    // ── Textures ──
    texContainer = loadTex("resources/textures/container.jpg");
    texEarth     = loadTex("resources/textures/earth.jpg");

    // ── Labels ──
    texLabel[0] = createLabelTexture("PC-A",   255,240,60);
    texLabel[1] = createLabelTexture("SWITCH",  60,230,120);
    texLabel[2] = createLabelTexture("ROUTER",  255,80,80);
    texLabel[3] = createLabelTexture("PC-B",    60,180,255);
    texLabel[4] = createLabelTexture("ASIA",    255,215,60);
    texLabel[5] = createLabelTexture("AMERICA",  60,215,255);

    // ── Init ──
    initLocalWaypoints();
    camera.resetLocal();

    float lastFrame=0.f;

    // ─────────────────────────────────────────────────────────
    // Render loop
    // ─────────────────────────────────────────────────────────
    while(!glfwWindowShouldClose(win)){
        float now=(float)glfwGetTime();
        float dt =now-lastFrame; lastFrame=now;

        processMovement(win,dt);

        if(appState==GLOBAL_MODE) earthRotDeg+=5.f*dt;

        // Packet update
        if(packetState==ANIMATING){
            if(appState==LOCAL_MODE){
                packetT+=LOCAL_SPEED*dt;
                if(packetT>=1.f){
                    packetT=0.f; packetSeg++;
                    int mx=(int)localWaypoints.size()-1;
                    if(packetSeg>=mx){
                        packetState=DELIVERED; activeDevice=4;
                        std::cout<<"\n[DELIVERED] → PC-B!\n";
                    }
                }
                if(packetState==ANIMATING){
                    int nd=segToDevice(packetSeg);
                    if(nd!=activeDevice){ activeDevice=nd; }
                    switch(nd){
                        case 1:printStatus("Sending from PC-A...");break;
                        case 2:printStatus("Processing at Switch...");break;
                        case 3:printStatus("Routing at Router...");break;
                        default:printStatus("Delivering to PC-B...");
                    }
                }
            } else if(appState==GLOBAL_MODE){
                packetT+=GLOBAL_SPEED*dt;
                if(packetT>=1.f){
                    packetT=1.f;packetState=DELIVERED;
                    std::cout<<"\n[DELIVERED] → Building B!\n";
                }
                printStatus("Global packet in transit...");
            }
        }
        if(packetState==DELIVERED) printStatus("Delivered! [SPACE] to resend.");

        glClearColor(0.04f,0.06f,0.11f,1.f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        glm::mat4 V=camera.getView();
        glm::mat4 P=glm::perspective(glm::radians(camera.fov),(float)SCR_WIDTH/SCR_HEIGHT,0.1f,100.f);

        // ── Render ──
        if(appState==LOCAL_MODE){
            drawLocalScene(mainShader,V,P);

            // Label positions (above each device)
            const float cW=0.38f, cH=0.40f;
            renderBillboard(billboardProg,texLabel[0],
                pos_PCA +glm::vec3(0,1.40f,0), 4*cW,cH,V,P);   // PC-A (4 chars)
            renderBillboard(billboardProg,texLabel[1],
                pos_SW  +glm::vec3(0,0.72f,0), 6*cW,cH,V,P);   // SWITCH (6)
            renderBillboard(billboardProg,texLabel[2],
                pos_RT  +glm::vec3(0,0.68f,0), 6*cW,cH,V,P);   // ROUTER (6)
            renderBillboard(billboardProg,texLabel[3],
                pos_PCB +glm::vec3(0,1.40f,0), 4*cW,cH,V,P);   // PC-B (4)

        } else if(appState==GLOBAL_MODE){
            drawGlobalScene(mainShader,V,P);

            const float earthR=2.0f, bH=0.52f;
            glm::vec3 nA=nA_base, nB=nB_base;
            const float cW=0.28f, cH=0.30f;
            renderBillboard(billboardProg,texLabel[4],
                nA*(earthR+bH)+nA*0.80f, 4*cW,cH,V,P); // ASIA
            renderBillboard(billboardProg,texLabel[5],
                nB*(earthR+bH)+nB*0.80f, 7*cW,cH,V,P); // AMERICA

        } else {
            // Menu preview
            glm::mat4 Vm=glm::lookAt(glm::vec3(0,7,18),glm::vec3(0,0.5f,0),{0,1,0});
            glm::mat4 Pm=glm::perspective(glm::radians(48.f),(float)SCR_WIDTH/SCR_HEIGHT,0.1f,100.f);
            drawLocalScene(mainShader,Vm,Pm);
        }

        glfwSwapBuffers(win);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

// ─────────────────────────────────────────────────────────────
// Callbacks
// ─────────────────────────────────────────────────────────────
void framebuffer_size_callback(GLFWwindow*,int w,int h){ glViewport(0,0,w,h); }

void mouse_callback(GLFWwindow*,double xp,double yp){
    if(!mouseButtonDown){firstMouse=true;return;}
    if(firstMouse){lastX=(float)xp;lastY=(float)yp;firstMouse=false;}
    float xo=((float)xp-lastX)*camera.sensitivity;
    float yo=(lastY-(float)yp) *camera.sensitivity;
    lastX=(float)xp;lastY=(float)yp;
    camera.yaw+=xo;
    camera.pitch=std::max(-89.f,std::min(89.f,camera.pitch+yo));
    camera.updateVectors();
}

void scroll_callback(GLFWwindow*,double,double y){
    camera.fov=std::max(10.f,std::min(90.f,camera.fov-(float)y*2.f));
}

void mouseButton_callback(GLFWwindow*,int btn,int act,int){
    if(btn==GLFW_MOUSE_BUTTON_RIGHT){
        mouseButtonDown=(act==GLFW_PRESS);
        if(mouseButtonDown)firstMouse=true;
    }
}

void key_callback(GLFWwindow* win,int key,int,int action,int){
    if(action!=GLFW_PRESS)return;
    if(key==GLFW_KEY_ESCAPE){glfwSetWindowShouldClose(win,true);return;}

    if(key==GLFW_KEY_1){
        appState=LOCAL_MODE;
        packetState=IDLE;packetT=0.f;packetSeg=0;activeDevice=0;
        initLocalWaypoints(); // re-init waypoints on mode switch
        camera.resetLocal();
        std::cout<<"\n[Mode Lokal] PCA─SW─RT─SW─PCB\n";
    }
    if(key==GLFW_KEY_2){
        appState=GLOBAL_MODE;
        packetState=IDLE;packetT=0.f;packetSeg=0;activeDevice=0;
        camera.resetGlobal();
        std::cout<<"\n[Mode Global] Bumi + Gedung\n";
    }
    if(key==GLFW_KEY_SPACE){
        if(appState==LOCAL_MODE||appState==GLOBAL_MODE){
            packetState=ANIMATING;
            packetT=0.f;packetSeg=0;activeDevice=1;
            if(appState==LOCAL_MODE) initLocalWaypoints();
            std::cout<<"\n[>] Mengirim paket...\n";
        }
    }
    if(key==GLFW_KEY_R){
        packetState=IDLE;packetT=0.f;packetSeg=0;activeDevice=0;
        if(appState==LOCAL_MODE) camera.resetLocal();
        if(appState==GLOBAL_MODE) camera.resetGlobal();
    }
    if(key==GLFW_KEY_M){
        appState=MENU;
        packetState=IDLE;packetT=0.f;packetSeg=0;activeDevice=0;
    }
}

void processMovement(GLFWwindow* win,float dt){
    if(appState==MENU)return;
    float spd=camera.speed*dt;
    glm::vec3 r=glm::normalize(glm::cross(camera.front,camera.up));
    if(glfwGetKey(win,GLFW_KEY_W)==GLFW_PRESS) camera.position+=camera.front*spd;
    if(glfwGetKey(win,GLFW_KEY_S)==GLFW_PRESS) camera.position-=camera.front*spd;
    if(glfwGetKey(win,GLFW_KEY_A)==GLFW_PRESS) camera.position-=r*spd;
    if(glfwGetKey(win,GLFW_KEY_D)==GLFW_PRESS) camera.position+=r*spd;
    if(glfwGetKey(win,GLFW_KEY_Q)==GLFW_PRESS) camera.position-=camera.up*spd;
    if(glfwGetKey(win,GLFW_KEY_E)==GLFW_PRESS) camera.position+=camera.up*spd;
}
