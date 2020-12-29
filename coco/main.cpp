#include "pch.h"
#include "AppDelegate.h"

int main() {
    gAppDelegate.Emplace();
    gAppDelegate->setTitle("HelloCpp");
    gAppDelegate->setFrameSize(2048, 1536);
    gAppDelegate->setFrameZoomFactor(0.4f);
    if (int r = gAppDelegate->initWindow()) return r;
    if (int r = gAppDelegate->initGL()) return r;
    return gAppDelegate->run();
}

/*

cocos2d-x 2.2.6 win32 启动流程：

1. 创建 AppDelegate ( 全局可访问其单例 )
2. 创建 CCEGLView ( 全局可访问其单例 ) 并 设置 窗口标题, SetFrameSize( 渲染分辨率, FrameZoomFactor 缩放比: 渲染分辨率 * 缩放比 = 实际窗口 size )
3. 持续运行 AppDelegate->run()

SetFrameSize 的过程中会创建 CCDirector


*/
