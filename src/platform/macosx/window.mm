#import "window.h"

#include "../platform_inner.h"
#include "../base_window.h"
#include "platform_data.h"

#include <scythe/desktop_application.h>
#include <scythe/keyboard.h>
#include <scythe/mouse.h>

//------------------------------------------------------------------------
// Delegate for window related notifications
//------------------------------------------------------------------------

@implementation ScytheWindowDelegate

- (BOOL)windowShouldClose:(id)sender
{
	scythe::Application* app = scythe::Application::GetInstance();
	app->Terminate(); // need_quit_ = true;
	return NO;
}

- (void)windowDidResize:(NSNotification *)notification
{
	scythe::DesktopApplication* app = scythe::DesktopApplication::GetInstance();
	scythe::platform::Data* data = scythe::platform::GetData(app);
	scythe::platform::Window* window = data->main_window;

	const NSRect viewRectPoints = [window->view frame];
	const NSRect viewRectPixels = [window->view convertRectToBacking:viewRectPoints];

	window->base->width = viewRectPixels.size.width;
	window->base->height = viewRectPixels.size.height;
	window->base->aspect_ratio = static_cast<float>(window->base->width) / static_cast<float>(window->base->height);
	window->base->fullscreen = false;

	scythe::WindowController* window_controller = app->GetWindowController();
	if (window_controller)
		window_controller->OnResize(window->base->width, window->base->height);
}

- (void)windowDidMove:(NSNotification *)notification
{
}

- (void)windowDidMiniaturize:(NSNotification *)notification
{
	scythe::DesktopApplication* app = scythe::DesktopApplication::GetInstance();
	scythe::platform::Data* data = scythe::platform::GetData(app);
	scythe::platform::Window* window = data->main_window;

	window->base->visible = false;
}

- (void)windowDidDeminiaturize:(NSNotification *)notification
{
	scythe::DesktopApplication* app = scythe::DesktopApplication::GetInstance();
	scythe::platform::Data* data = scythe::platform::GetData(app);
	scythe::platform::Window* window = data->main_window;

	window->base->visible = true;
}

- (void)windowDidBecomeKey:(NSNotification *)notification
{
	scythe::DesktopApplication* app = scythe::DesktopApplication::GetInstance();
	scythe::platform::Data* data = scythe::platform::GetData(app);
	scythe::platform::Window* window = data->main_window;

	window->base->active = true;
}

- (void)windowDidResignKey:(NSNotification *)notification
{
	scythe::DesktopApplication* app = scythe::DesktopApplication::GetInstance();
	scythe::platform::Data* data = scythe::platform::GetData(app);
	scythe::platform::Window* window = data->main_window;

	window->base->active = false;
}

- (NSSize)window:(NSWindow *)window willUseFullScreenContentSize:(NSSize)proposedSize
{
	NSRect screenRect = [[NSScreen mainScreen] frame];
	return screenRect.size;
}

@end // ScytheWindowDelegate

// Translates OS X key modifiers to engine ones
static scythe::KeyModifiers TranslateModifiers(NSUInteger mods)
{
	scythe::KeyModifiers modifier;
#if __MAC_OS_X_VERSION_MIN_REQUIRED < 101200
	if (mods & NSShiftKeyMask)
		modifier |= scythe::KeyboardKeyModifier::kShift;
	if (mods & NSControlKeyMask)
		modifier |= scythe::KeyboardKeyModifier::kControl;
	if (mods & NSAlternateKeyMask)
		modifier |= scythe::KeyboardKeyModifier::kAlt;
	if (mods & NSCommandKeyMask)
		modifier |= scythe::KeyboardKeyModifier::kSuper;
#else
	if (mods & NSEventModifierFlagShift)
		modifier |= scythe::KeyboardKeyModifier::kShift;
	if (mods & NSEventModifierFlagControl)
		modifier |= scythe::KeyboardKeyModifier::kControl;
	if (mods & NSEventModifierFlagOption)
		modifier |= scythe::KeyboardKeyModifier::kAlt;
	if (mods & NSEventModifierFlagCommand)
		modifier |= scythe::KeyboardKeyModifier::kSuper;
#endif
	return modifier;
}

// Translates OS X mouse button numbers to engine ones
static scythe::MouseButton TranslateMouseButton(int button)
{
	scythe::MouseButton translated = scythe::MouseButton::kUnknown;
	switch (button)
	{
		case 2:
			translated = scythe::MouseButton::kMiddle;
			break;
	}
	return translated;
}

// Process any mouse button down/up input
static void ProcessMouseDown(scythe::MouseButton mouse_button, bool down, NSUInteger mods)
{
	scythe::DesktopApplication* app = scythe::DesktopApplication::GetInstance();
	scythe::KeyboardState& keyboard_state = app->GetKeyboardState();
	scythe::MouseState& mouse_state = app->GetMouseState();
	scythe::MouseController* mouse_controller = app->GetMouseController();

	keyboard_state.modifiers = ::TranslateModifiers(mods);
	mouse_state.SetButtonDown(mouse_button, true);
	if (mouse_controller)
	{
		if (down)
			mouse_controller->OnMouseDown(mouse_button);
		else
			mouse_controller->OnMouseUp(mouse_button);
	}
}

//------------------------------------------------------------------------
// Content view for window
//------------------------------------------------------------------------

@implementation ScytheContentView

-(id)init
{
	self = [super initWithFrame:NSMakeRect(0, 0, 1, 1)
					pixelFormat:nil];
	return self;
}

-(void)dealloc
{
	[super dealloc];
}

- (BOOL)isOpaque
{
	return YES;
}

- (BOOL)canBecomeKeyView
{
	return YES;
}

- (BOOL)acceptsFirstResponder
{
	return YES;
}

- (void)renewGState
{
	// Called whenever graphics state updated (such as window resize)
	
	// OpenGL rendering is not synchronous with other rendering on the OSX.
	// Therefore, call disableScreenUpdatesUntilFlush so the window server
	// doesn't render non-OpenGL content in the window asynchronously from
	// OpenGL content, which could cause flickering.  (non-OpenGL content
	// includes the title bar and drawing done by the app with other APIs)
	[[self window] disableScreenUpdatesUntilFlush];
	
	[super renewGState];
}

- (void)viewDidChangeBackingProperties
{
	scythe::DesktopApplication* app = scythe::DesktopApplication::GetInstance();
	scythe::WindowController* window_controller = app->GetWindowController();
	scythe::platform::Data* data = scythe::platform::GetData(app);
	scythe::platform::Window* window = data->main_window;

	const NSRect viewRectPoints = [window->view frame];
	const NSRect viewRectPixels = [window->view convertRectToBacking:viewRectPoints];

	window->base->width = viewRectPixels.size.width;
	window->base->height = viewRectPixels.size.height;
	window->base->aspect_ratio = static_cast<float>(window->base->width) / static_cast<float>(window->base->height);

	if (window_controller)
		window_controller->OnResize(window->base->width, window->base->height);
}

// ----- Keyboard events -----

- (void) keyDown:(NSEvent *)theEvent
{
	const unsigned short key_code = [theEvent keyCode];
	const scythe::KeyModifiers modifiers = ::TranslateModifiers([theEvent modifierFlags]);
	
	scythe::DesktopApplication* app = scythe::DesktopApplication::GetInstance();
	scythe::KeyboardController* keyboard_controller = app->GetKeyboardController();
	scythe::KeyboardState& keyboard_state = app->GetKeyboardState();
	const scythe::KeyboardKey translated_key = keyboard_state.GetKeyByCode(key_code);

	if (keyboard_controller)
		keyboard_controller->OnKeyDown(translated_key, modifiers);

	keyboard_state.SetKeyDown(translated_key, true);
	keyboard_state.modifiers = modifiers;

	NSString* characters = [theEvent characters];
	NSUInteger i, length = [characters length];
	
	for (i = 0; i < length; ++i)
	{
		const unichar codepoint = [characters characterAtIndex:i];
		if ((codepoint & 0xff00) == 0xf700)
			continue;
		
		if (scythe::IsGoodChar(codepoint))
		{
			if (keyboard_controller)
				keyboard_controller->OnChar(codepoint);
		}
	}
}

- (void) keyUp:(NSEvent *)theEvent
{
	const unsigned short key_code = [theEvent keyCode];
	const scythe::KeyModifiers modifiers = ::TranslateModifiers([theEvent modifierFlags]);

	scythe::DesktopApplication* app = scythe::DesktopApplication::GetInstance();
	scythe::KeyboardController* keyboard_controller = app->GetKeyboardController();
	scythe::KeyboardState& keyboard_state = app->GetKeyboardState();
	const scythe::KeyboardKey translated_key = keyboard_state.GetKeyByCode(key_code);

	if (keyboard_controller)
		keyboard_controller->OnKeyUp(translated_key, modifiers);

	keyboard_state.SetKeyDown(translated_key, false);
	keyboard_state.modifiers = modifiers;
}

- (void) flagsChanged:(NSEvent *)theEvent
{
	const unsigned short key_code = [theEvent keyCode];
#if __MAC_OS_X_VERSION_MIN_REQUIRED < 101200
	const int kFlagsMask = NSDeviceIndependentModifierFlagsMask;
#else
	const int kFlagsMask = NSEventModifierFlagDeviceIndependentFlagsMask;
#endif
	const scythe::KeyModifiers modifiers = ::TranslateModifiers([theEvent modifierFlags] & kFlagsMask);

	scythe::DesktopApplication* app = scythe::DesktopApplication::GetInstance();
	scythe::KeyboardController* keyboard_controller = app->GetKeyboardController();
	scythe::KeyboardState& keyboard_state = app->GetKeyboardState();
	const scythe::KeyboardKey translated_key = keyboard_state.GetKeyByCode(key_code);
	
	bool press;
	if (keyboard_state.modifiers == modifiers)
	{
		press = ! keyboard_state.IsKeyDown(translated_key);
		keyboard_state.SetKeyDown(translated_key, press);
	}
	else
	{
		press = modifiers > keyboard_state.modifiers;
		keyboard_state.modifiers = modifiers;
	}
	if (keyboard_controller)
	{
		if (press)
			keyboard_controller->OnKeyDown(translated_key, modifiers);
		else
			keyboard_controller->OnKeyUp(translated_key, modifiers);
	}
}

// ----- Mouse events -----

- (void) mouseDown:(NSEvent *)theEvent
{
	::ProcessMouseDown(scythe::MouseButton::kLeft, true, [theEvent modifierFlags]);
}

- (void) mouseUp:(NSEvent *)theEvent
{
	::ProcessMouseDown(scythe::MouseButton::kLeft, false, [theEvent modifierFlags]);
}

- (void) mouseMoved:(NSEvent *)theEvent
{
	scythe::DesktopApplication* app = scythe::DesktopApplication::GetInstance();
	scythe::MouseState& mouse_state = app->GetMouseState();
	scythe::MouseController* mouse_controller = app->GetMouseController();
	scythe::platform::Data* data = scythe::platform::GetData(app);
	scythe::platform::Window* window = data->main_window;

	NSPoint pos = [theEvent locationInWindow];
	NSRect pointRect = NSMakeRect(pos.x, pos.y, 1, 1);
	NSRect pixelRect = [window->view convertRectToBacking:pointRect];

	mouse_state.delta_x = [theEvent deltaX];
	mouse_state.delta_y = [theEvent deltaY];
	mouse_state.x = pixelRect.origin.x;
	mouse_state.y = pixelRect.origin.y;

	if (mouse_controller)
		mouse_controller->OnMouseMove();
}

- (void) mouseDragged:(NSEvent *)theEvent
{
	[self mouseMoved:theEvent];
}

- (void) rightMouseDown:(NSEvent *)theEvent
{
	::ProcessMouseDown(scythe::MouseButton::kRight, true, [theEvent modifierFlags]);
}

- (void) rightMouseUp:(NSEvent *)theEvent
{
	::ProcessMouseDown(scythe::MouseButton::kRight, false, [theEvent modifierFlags]);
}

- (void) rightMouseDragged:(NSEvent *)theEvent
{
	[self mouseMoved:theEvent];
}

- (void) otherMouseDown:(NSEvent *)theEvent
{
	scythe::MouseButton button = ::TranslateMouseButton((int)[theEvent buttonNumber]);
	::ProcessMouseDown(button, true, [theEvent modifierFlags]);
}

- (void) otherMouseUp:(NSEvent *)theEvent
{
	scythe::MouseButton button = ::TranslateMouseButton((int)[theEvent buttonNumber]);
	::ProcessMouseDown(button, false, [theEvent modifierFlags]);
}

- (void) otherMouseDragged:(NSEvent *)theEvent
{
	[self mouseMoved:theEvent];
}

- (void) scrollWheel:(NSEvent *)theEvent
{
	float delta_x = [theEvent scrollingDeltaX];
	float delta_y = [theEvent scrollingDeltaY];
	if ([theEvent hasPreciseScrollingDeltas])
	{
		delta_x *= 0.1f;
		delta_y *= 0.1f;
	}
	scythe::DesktopApplication* app = scythe::DesktopApplication::GetInstance();
	scythe::MouseController* mouse_controller = app->GetMouseController();
	if (mouse_controller)
		mouse_controller->OnScroll(delta_x, delta_y);
}

@end // ScytheContentView

//------------------------------------------------------------------------
// Scythe window class
//------------------------------------------------------------------------

@implementation ScytheWindow

- (id)init
{
	self = [super init];
	if (self)
		fullscreenWindow = nil;
	return self;
}

- (void)dealloc
{
	if (fullscreenWindow)
	{
		[fullscreenWindow release];
		fullscreenWindow = nil;
	}
	[super dealloc];
}

- (BOOL)canBecomeKeyWindow
{
	// Required for NSBorderlessWindowMask windows
	return YES;
}

- (void)toggleFullScreen:(id)sender
{
	scythe::platform::window::ToggleFullscreen();
}

- (void)goFullscreen
{
	// If app is already fullscreen...
	if(fullscreenWindow)
	{
		//...don't do anything
		return;
	}
	
	// Allocate a new fullscreen window
	fullscreenWindow = [[ScytheFullscreenWindow alloc] init];
	
	// Resize the view to screensize
	NSRect viewRect = [fullscreenWindow frame];

	scythe::DesktopApplication* app = scythe::DesktopApplication::GetInstance();
	scythe::platform::Data* data = scythe::platform::GetData(app);
	scythe::platform::Window* window = data->main_window;
	
	// Set the view to the size of the fullscreen window
	[window->view setFrameSize: viewRect.size];
	
	// Set the view in the fullscreen window
	[fullscreenWindow setContentView:window->view];
	
	// Hide non-fullscreen window so it doesn't show up when switching out
	// of this app (i.e. with CMD-TAB)
	[window->object orderOut:self];
	
	// Show the window and make it the key window for input
	[fullscreenWindow makeKeyAndOrderFront:self];
	[fullscreenWindow setAcceptsMouseMovedEvents:YES];
}

- (void)goWindow
{
	// If controller doesn't have a full screen window...
	if(fullscreenWindow == nil)
	{
		//...app is already windowed so don't do anything
		return;
	}

	scythe::DesktopApplication* app = scythe::DesktopApplication::GetInstance();
	scythe::platform::Data* data = scythe::platform::GetData(app);
	scythe::platform::Window* window = data->main_window;
	
	// Get the rectangle of the original window
	NSRect viewRect = [window->object frame];
	
	// Set the view rect to the new size
	[window->view setFrame:viewRect];
	
	// Set the content of the orginal window to the view
	[window->object setContentView:window->view];
	
	// Show the window and make it the key window for input
	[window->object makeKeyAndOrderFront:self];
	
	// Release the fullscreen window
	[fullscreenWindow release];
	
	// Ensure we set fullscreen Window to nil so our checks for
	// windowed vs. fullscreen mode elsewhere are correct
	fullscreenWindow = nil;
}

@end // ScytheWindow
