#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

PSP_MODULE_INFO("gutest", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_VFPU | THREAD_ATTR_USER);

#define BUFFER_WIDTH 512
#define BUFFER_HEIGHT 272
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT BUFFER_HEIGHT

typedef struct
{
    float u, v;
    uint32_t colour;
    float x, y, z;
} TextureVertex;

typedef struct
{
    int width, height;
    uint32_t * data;
} Texture;



char list[0x20000] __attribute__((aligned(64)));

void * fbp0;
void * fbp1;
Texture texture;

void initGu(){
    sceGuInit();

    fbp0 = guGetStaticVramBuffer(BUFFER_WIDTH, BUFFER_HEIGHT, GU_PSM_8888);
    fbp1 = guGetStaticVramBuffer(BUFFER_WIDTH, BUFFER_HEIGHT, GU_PSM_8888);

    //Set up buffers
    sceGuStart(GU_DIRECT, list);
    sceGuDrawBuffer(GU_PSM_8888, fbp0, BUFFER_WIDTH);
    sceGuDispBuffer(SCREEN_WIDTH,SCREEN_HEIGHT,fbp1, BUFFER_WIDTH);
    
    // We do not care about the depth buffer in this example
    sceGuDepthBuffer(fbp0, 0); // Set depth buffer to a length of 0
    sceGuDisable(GU_DEPTH_TEST); // Disable depth testing

    //Set up viewport
    sceGuOffset(2048 - (SCREEN_WIDTH / 2), 2048 - (SCREEN_HEIGHT / 2));
    sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Start a new frame and enable the display
    sceGuFinish();
    sceGuDisplay(GU_TRUE);
}

void endGu(){
    sceGuDisplay(GU_FALSE);
    sceGuTerm();
}

void startFrame(){
    sceGuStart(GU_DIRECT, list);
    sceGuClearColor(0xFFFFFFFF); // White background
    sceGuClear(GU_COLOR_BUFFER_BIT);
}

void endFrame(){
    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();
}

void drawTexture(float x, float y, float w, float h) {

    TextureVertex vertices[2] = {
        {0.0f, 0.0f, 0xFFFFFFFF, x, y, 0.0f},
        {w, h, 0xFFFFFFFF, x + w, y + h, 0.0f},
    };

    sceGuTexMode(GU_PSM_8888, 0, 0, GU_FALSE);
    sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGB);
    sceGuTexImage(0, texture.width, texture.height, texture.width, texture.data);

	sceGuEnable(GU_TEXTURE_2D); 
    sceGuDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, 0, vertices);
    sceGuDisable(GU_TEXTURE_2D);
}


int main() {
    initGu();

    texture.data = (uint32_t *) stbi_load("grass.png", &texture.width, &texture.height, NULL, 4);

    int running = 1;
    while(running){
        startFrame();

        drawTexture(SCREEN_WIDTH / 2 - 8, SCREEN_HEIGHT / 2 - 8, 16, 16);

        endFrame();
    }

    return 0;
}
