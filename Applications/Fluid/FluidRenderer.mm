#import "FluidRenderer.h"
#include "FluidRendererCPP.h"

@interface FluidRenderer()
{
    FluidRendererCPP* _rendererCPP;
}
@end

@implementation FluidRenderer

- (void) dealloc
{
    if (_rendererCPP)
    {
        delete _rendererCPP;
        _rendererCPP = nullptr;
    }
}

- (id) init
{
    self = [super init];
    if (self)
    {
        _rendererCPP = new FluidRendererCPP();
    }
    return self;
}

- (void) render:(CGSize)size
{
    _rendererCPP->render(size);
}


@end
