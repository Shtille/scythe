#import <Cocoa/Cocoa.h>

// Delegate for window related notifications
@interface ScytheWindowDelegate : NSObject
@end

// Content view class for the Scythe window
@interface ScytheContentView : NSOpenGLView

-(id)init;

@end

// Scythe window class
@interface ScytheWindow : NSWindow
{
	ScytheFullscreenWindow * fullscreenWindow;
}
- (void)goFullscreen;
- (void)goWindow;
@end