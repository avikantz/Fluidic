#import "FluidOpenGLView.h"
#import "FluidRenderer.h"

#include <OpenGL/gl3.h>

@interface FluidOpenGLView()
{
    FluidRenderer* _renderer;
}
@end

@implementation FluidOpenGLView

- (void) dealloc
{
}

- (id) initWithFrame:(NSRect)frameRect
{
	NSOpenGLPixelFormatAttribute attribs[] =
	{
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
		NSOpenGLPFANoRecovery,
		NSOpenGLPFAWindow,
		NSOpenGLPFAAccelerated,
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFAColorSize, 24,
		NSOpenGLPFAAlphaSize, 8,
		NSOpenGLPFADepthSize, 24,
		NSOpenGLPFAStencilSize, 8,
		NSOpenGLPFAAccumSize, 0,
		0
	};
    
	NSOpenGLPixelFormat* fmt = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
    self = [super initWithFrame:frameRect pixelFormat:fmt];
    if (self)
    {
        // Initialization
    }
    return self;
}

- (void) awakeFromNib
{
    NSTimer* refreshTimer = [NSTimer timerWithTimeInterval:1.0 / 50.0
                                target:self
                                selector:@selector(timerEvent:)
                                userInfo:nil repeats:YES];
    [[NSRunLoop mainRunLoop] addTimer:refreshTimer forMode:NSDefaultRunLoopMode];
}

- (void) timerEvent:(NSTimer*)timer
{
    [self setNeedsDisplay:YES];
}

- (void) prepareOpenGL
{
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    _renderer = [[FluidRenderer alloc] init];
}

- (void) clearGLContext
{
    _renderer = nil;
    [super clearGLContext];
}

-(void) drawRect:(NSRect)bounds
{
    [_renderer render:[self bounds].size];
    [[self openGLContext] flushBuffer];
}

@end
