#import "fullscreen_window.h"
#import "window.h"

#include <crt_externs.h> // for _NSGetProgname()
#include <wchar.h> // for wcslen

#include "../platform_inner.h"
#include "../base_window.h"
#include "platform_data.h"

#include <scythe/desktop_application.h>
#include <scythe/log.h>

static inline scythe::platform::Window* GetMainWindow()
{
	scythe::Application* app = scythe::Application::GetInstance();
	scythe::platform::Data* data = scythe::platform::GetData(app);
	return data->main_window;
}

//------------------------------------------------------------------------
// Delegate for application related notifications
//------------------------------------------------------------------------

@interface ScytheApplicationDelegate : NSObject
@end

@implementation ScytheApplicationDelegate

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
	scythe::Application* app = scythe::Application::GetInstance();
	app->Terminate(); // need_quit_ = true;
	return NSTerminateCancel;
}

- (void)applicationDidChangeScreenParameters:(NSNotification *) notification
{
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
	[NSApp stop:nil];

	// Post empty event
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	NSEvent* event = [NSEvent 
#if __MAC_OS_X_VERSION_MIN_REQUIRED < 101200
							  otherEventWithType:NSApplicationDefined
#else
							  otherEventWithType:NSEventTypeApplicationDefined
#endif
										location:NSMakePoint(0, 0)
								   modifierFlags:0
									   timestamp:0
									windowNumber:0
										 context:nil
										 subtype:0
										   data1:0
										   data2:0];
	[NSApp postEvent:event atStart:YES];
	[pool drain];
}

@end // ScytheApplicationDelegate

//------------------------------------------------------------------------
// Scythe application class
//------------------------------------------------------------------------

@interface ScytheApplication : NSApplication
@end

@implementation ScytheApplication

// From http://cocoadev.com/index.pl?GameKeyboardHandlingAlmost
// This works around an AppKit bug, where key up events while holding
// down the command key don't get sent to the key window.
- (void)sendEvent:(NSEvent *)event
{
#if __MAC_OS_X_VERSION_MIN_REQUIRED < 101200
	const NSEventType kEventTypeKeyUp = NSKeyUp;
	const NSEventModifierFlags kCommandKeyMask = NSCommandKeyMask;
#else
	const NSEventType kEventTypeKeyUp = NSEventTypeKeyUp;
	const NSEventModifierFlags kCommandKeyMask = NSEventModifierFlagCommand;
#endif
	if ([event type] == kEventTypeKeyUp && ([event modifierFlags] & kCommandKeyMask))
		[[self keyWindow] sendEvent:event];
	else
		[super sendEvent:event];
}

@end

// Try to figure out what the calling application is called
//
static NSString* FindAppName(void)
{
	size_t i;
	NSDictionary* infoDictionary = [[NSBundle mainBundle] infoDictionary];

	// Keys to search for as potential application names
	NSString* nameKeys[] =
	{
		@"CFBundleDisplayName",
		@"CFBundleName",
		@"CFBundleExecutable",
	};

	for (i = 0;  i < sizeof(nameKeys) / sizeof(nameKeys[0]);  i++)
	{
		id name = [infoDictionary objectForKey:nameKeys[i]];
		if (name &&
			[name isKindOfClass:[NSString class]] &&
			![name isEqualToString:@""])
		{
			return name;
		}
	}

	char** progname = _NSGetProgname();
	if (progname && *progname)
		return [NSString stringWithUTF8String:*progname];

	// Really shouldn't get here
	return @"Scythe Application";
}

// Set up the menu bar (manually)
// This is nasty, nasty stuff -- calls to undocumented semi-private APIs that
// could go away at any moment, lots of stuff that really should be
// localize(d|able), etc.  Loading a nib would save us this horror, but that
// doesn't seem like a good thing to require of Shtille Engine users.
//
static void CreateMenuBar()
{
#if __MAC_OS_X_VERSION_MIN_REQUIRED < 101200
	const NSEventModifierFlags kAlternateKeyMask = NSAlternateKeyMask;
	const NSEventModifierFlags kCommandKeyMask = NSCommandKeyMask;
	const NSEventModifierFlags kControlKeyMask = NSControlKeyMask;
#else
	const NSEventModifierFlags kAlternateKeyMask = NSEventModifierFlagOption;
	const NSEventModifierFlags kCommandKeyMask = NSEventModifierFlagCommand;
	const NSEventModifierFlags kControlKeyMask = NSEventModifierFlagControl;
#endif

	NSString* appName = ::FindAppName();

	NSMenu* bar = [[NSMenu alloc] init];
	[NSApp setMainMenu:bar];

	NSMenuItem* appMenuItem =
		[bar addItemWithTitle:@"" action:NULL keyEquivalent:@""];
	NSMenu* appMenu = [[NSMenu alloc] init];
	[appMenuItem setSubmenu:appMenu];

	[appMenu addItemWithTitle:[NSString stringWithFormat:@"About %@", appName]
					   action:@selector(orderFrontStandardAboutPanel:)
				keyEquivalent:@""];
	[appMenu addItem:[NSMenuItem separatorItem]];
	NSMenu* servicesMenu = [[NSMenu alloc] init];
	[NSApp setServicesMenu:servicesMenu];
	[[appMenu addItemWithTitle:@"Services"
					   action:NULL
				keyEquivalent:@""] setSubmenu:servicesMenu];
	[appMenu addItem:[NSMenuItem separatorItem]];
	[appMenu addItemWithTitle:[NSString stringWithFormat:@"Hide %@", appName]
					   action:@selector(hide:)
				keyEquivalent:@"h"];
	[[appMenu addItemWithTitle:@"Hide Others"
					   action:@selector(hideOtherApplications:)
				keyEquivalent:@"h"]
		setKeyEquivalentModifierMask:kAlternateKeyMask | kCommandKeyMask];
	[appMenu addItemWithTitle:@"Show All"
					   action:@selector(unhideAllApplications:)
				keyEquivalent:@""];
	[appMenu addItem:[NSMenuItem separatorItem]];
	[appMenu addItemWithTitle:[NSString stringWithFormat:@"Quit %@", appName]
					   action:@selector(terminate:)
				keyEquivalent:@"q"];

	NSMenuItem* windowMenuItem =
		[bar addItemWithTitle:@"" action:NULL keyEquivalent:@""];
	NSMenu* windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];
	[NSApp setWindowsMenu:windowMenu];
	[windowMenuItem setSubmenu:windowMenu];

	[windowMenu addItemWithTitle:@"Minimize"
						  action:@selector(performMiniaturize:)
				   keyEquivalent:@"m"];
	[windowMenu addItemWithTitle:@"Zoom"
						  action:@selector(performZoom:)
				   keyEquivalent:@""];
	[windowMenu addItem:[NSMenuItem separatorItem]];
	[windowMenu addItemWithTitle:@"Bring All to Front"
						  action:@selector(arrangeInFront:)
				   keyEquivalent:@""];

	[windowMenu addItem:[NSMenuItem separatorItem]];
	[[windowMenu addItemWithTitle:@"Enter Full Screen"
						   action:@selector(toggleFullScreen:)
					keyEquivalent:@"f"]
		setKeyEquivalentModifierMask:kControlKeyMask | kCommandKeyMask];

	// Prior to Snow Leopard, we need to use this oddly-named semi-private API
	// to get the application menu working properly.
	SEL setAppleMenuSelector = NSSelectorFromString(@"setAppleMenu:");
	[NSApp performSelector:setAppleMenuSelector withObject:appMenu];
}

static bool InitializeAppKit()
{
	if (NSApp)
		return true;

	// Implicitly create shared NSApplication instance
	[ScytheApplication sharedApplication];

	// In case we are unbundled, make us a proper UI application
	[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

	// Menu bar setup must go between sharedApplication above and
	// finishLaunching below, in order to properly emulate the behavior
	// of NSApplicationMain
	::CreateMenuBar();

	scythe::Application* app = scythe::Application::GetInstance();
	scythe::platform::Data* data = scythe::platform::GetData(app);

	// There can only be one application delegate, but we allocate it the
	// first time a window is created to keep all window code in this file
	data->app_delegate = [[ScytheApplicationDelegate alloc] init];
	if (data->app_delegate == nil)
	{
		scythe::Error("Cocoa: failed to create application delegate");
		return false;
	}

	[NSApp setDelegate:data->app_delegate];
	[NSApp run];

	return true;
}

static bool CreateWindow()
{
	scythe::DesktopApplication* app = scythe::DesktopApplication::GetInstance();
	scythe::platform::Data* data = scythe::platform::GetData(app);
	scythe::platform::Window* window = data->main_window;

	window->delegate = [[ScytheWindowDelegate alloc] init];
	if (window->delegate == nil)
	{
		scythe::Error("Cocoa: Failed to create window delegate");
		return false;
	}

	unsigned int styleMask = 0;

	if (app->IsDecorated())
	{
#if __MAC_OS_X_VERSION_MIN_REQUIRED < 101200
		styleMask = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask;
#else
		styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;
#endif

		if (app->IsResizable())
#if __MAC_OS_X_VERSION_MIN_REQUIRED < 101200
			styleMask |= NSResizableWindowMask;
#else
			styleMask |= NSWindowStyleMaskResizable;
#endif
	}
	else
#if __MAC_OS_X_VERSION_MIN_REQUIRED < 101200
		styleMask = NSBorderlessWindowMask;
#else
		styleMask = NSWindowStyleMaskBorderless;
#endif

	// We will create fullscreen window separately
	int width = app->GetInitialWidth();
	int height = app->GetInitialHeight();
	NSRect contentRect = NSMakeRect(0, 0, width, height);

	window->object = [[ScytheWindow alloc]
		initWithContentRect:contentRect
				  styleMask:styleMask
					backing:NSBackingStoreBuffered
					  defer:NO];

	if (window->object == nil)
	{
		scythe::Error("Cocoa: Failed to create window");
		return false;
	}

	if (floor(NSAppKitVersionNumber) > NSAppKitVersionNumber10_6)
	{
		if (app->IsResizable())
			[window->object setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
	}

	std::wstring appTitle = app->GetTitle();
	const wchar_t* title = appTitle.c_str();
	NSUInteger byteLength = wcslen(title) * sizeof(wchar_t);
	NSString *titleStr = [[NSString alloc] initWithBytes:title
				                                  length:byteLength
				                                encoding:NSUTF32LittleEndianStringEncoding];

	[window->object setTitle:titleStr];
	[window->object setDelegate:window->delegate];
	[window->object setAcceptsMouseMovedEvents:YES];

	[titleStr release];

	if (floor(NSAppKitVersionNumber) > NSAppKitVersionNumber10_6)
		[window->object setRestorable:NO];

	// Store window size in pixels
	// NSScreen *screen = [NSScreen mainScreen];
	// CGFloat scale = [screen backingScaleFactor]; // 2.0 for Retina and 1.0 for older screens
	// NSRect windowRectInPoints = [NSWindow frameRectForContentRect:contentRect styleMask:styleMask];
	// CGFloat pixelWidth = windowRectInPoints.size.width * scale;
	// CGFloat pixelHeight = windowRectInPoints.size.height * scale;

	// window->base->width = pixelWidth;
	// window->base->height = pixelHeight;
	// window->base->aspect_ratio = static_cast<float>(pixelWidth) / static_cast<float>(pixelHeight);
	// window->base->active = true;
	// window->base->visible = false;
	// window->base->fullscreen = false;

	return true;
}

//------------------------------------------------------------------------
// Scythe platform API
//------------------------------------------------------------------------

namespace scythe {

	const BaseWindow* GetBaseWindow(const Application* app)
	{
		const platform::Window* window = platform::GetWindow(app);
		return window->base;
	}

	namespace platform {

		Data* CreateData()
		{
			Data* data = new Data();
			data->event_source = nullptr;
			data->app_delegate = nil;
			data->autorelease_pool = nil;
			data->main_window = nullptr;
			return data;
		}
		void DestroyData(Data* data)
		{
			delete data;
		}
		Data* GetData(Application* app)
		{
			return reinterpret_cast<Data*>(app->platform_data_);
		}
		const Data* GetData(const Application* app)
		{
			return reinterpret_cast<const Data*>(app->platform_data_);
		}
		const Window* GetWindow(const Application* app)
		{
			const Data* data = GetData(app);
			return data->main_window;
		}

		bool Initialize()
		{
			Application* app = Application::GetInstance();
			Data* data = GetData(app);

			data->app_delegate = nil;
			data->autorelease_pool = [[NSAutoreleasePool alloc] init];

			data->event_source = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
			if (!data->event_source)
				return false;

			CGEventSourceSetLocalEventsSuppressionInterval(data->event_source, 0.0);
			return true;
		}
		void Deinitialize()
		{
			Application* app = Application::GetInstance();
			Data* data = GetData(app);

			if (data->event_source)
			{
				CFRelease(data->event_source);
				data->event_source = NULL;
			}

			if (data->app_delegate)
			{
				[NSApp setDelegate:nil];
				[data->app_delegate release];
				data->app_delegate = nil;
			}

			[data->autorelease_pool release];
			data->autorelease_pool = nil;
		}

		void PollEvents()
		{
			Application* app = Application::GetInstance();
			Data* data = GetData(app);

		#if __MAC_OS_X_VERSION_MIN_REQUIRED < 101200
			const NSEventMask kEventMaskAny = NSAnyEventMask;
		#else
			const NSEventMask kEventMaskAny = NSEventMaskAny;
		#endif
			while (true)
			{
				NSEvent* event = [NSApp nextEventMatchingMask:kEventMaskAny
													untilDate:[NSDate distantPast]
													   inMode:NSDefaultRunLoopMode
													  dequeue:YES];
				if (event == nil)
					break;

				[NSApp sendEvent:event];
			}

			[data->autorelease_pool drain];
			data->autorelease_pool = [[NSAutoreleasePool alloc] init];
		}
		void ChangeDirectoryToResources()
		{
			char resourcesPath[MAXPATHLEN];
	
			CFBundleRef bundle = CFBundleGetMainBundle();
			if (!bundle)
				return;
			
			CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(bundle);
			
			CFStringRef last = CFURLCopyLastPathComponent(resourcesURL);
			if (CFStringCompare(CFSTR("Resources"), last, 0) != kCFCompareEqualTo)
			{
				CFRelease(last);
				CFRelease(resourcesURL);
				return;
			}
			
			CFRelease(last);
			
			if (!CFURLGetFileSystemRepresentation(resourcesURL,
												  true,
												  (UInt8*) resourcesPath,
												  MAXPATHLEN))
			{
				CFRelease(resourcesURL);
				return;
			}
			
			CFRelease(resourcesURL);
			
			chdir(resourcesPath);
		}

		// Window functions
		namespace window {

			bool Create()
			{
				DesktopApplication* app = DesktopApplication::GetInstance();
				Data* data = GetData(app);

				// Create window instance
				Window* window = new Window();
				data->main_window = window;
				if (!data->main_window)
					return false;

				if (!::InitializeAppKit())
					return false;

				if (!::CreateWindow())
					return false;

				window->view = [[ScytheContentView alloc] init];

				// Opt-In to Retina resolution
				if (floor(NSAppKitVersionNumber) > NSAppKitVersionNumber10_6)
					[window->view setWantsBestResolutionOpenGLSurface:YES];

				[window->object setContentView:window->view];

				// Create a fullscreen window initially
				if (app->IsFullscreen())
					[window->object goFullscreen];

				const NSRect viewRectPoints = [window->view frame];
				const NSRect viewRectPixels = [window->view convertRectToBacking:viewRectPoints];

				window->base->width = viewRectPixels.size.width;
				window->base->height = viewRectPixels.size.height;
				window->base->aspect_ratio = static_cast<float>(window->base->width) / static_cast<float>(window->base->height);
				window->base->active = true;
				window->base->visible = true;
				window->base->fullscreen = app->IsFullscreen();

				return true;
			}
			void Destroy()
			{
				DesktopApplication* app = DesktopApplication::GetInstance();
				Data* data = GetData(app);
				Window* window = data->main_window;

				[window->object orderOut:nil];

				[window->object setDelegate:nil];
				[window->delegate release];
				window->delegate = nil;

				[window->view release];
				window->view = nil;

				[window->object close];
				window->object = nil;

				// Delete its instance
				if (data->main_window)
				{
					delete data->main_window;
					data->main_window = nullptr;
				}
			}
			void Terminate()
			{
				Application* app = Application::GetInstance();
				app->Terminate(); // need_quit_ = true;
				// [[NSApplication sharedApplication] terminate:nil];
			}

			void ToggleFullscreen(void)
			{
				Window* window = ::GetMainWindow();
				if (window->base->fullscreen) // fullscreen -> windowed
				{
					MakeWindowed();
				}
				else // windowed -> fullscreen
				{
					MakeFullscreen();
				}
			}
			bool MakeFullscreen()
			{
				DesktopApplication* app = DesktopApplication::GetInstance();
				Data* data = GetData(app);
				Window* window = data->main_window;

				if (window->base->fullscreen)
					return true;

				[window->object goFullscreen];

				const NSRect viewRectPoints = [window->view frame];
				const NSRect viewRectPixels = [window->view convertRectToBacking:viewRectPoints];

				window->base->width = viewRectPixels.size.width;
				window->base->height = viewRectPixels.size.height;
				window->base->aspect_ratio = static_cast<float>(window->base->width) / static_cast<float>(window->base->height);
				window->base->fullscreen = true;

				WindowController* window_controller = app->GetWindowController();
				if (window_controller)
					window_controller->OnResize(window->base->width, window->base->height);

				return true;
			}
			void MakeWindowed()
			{
				DesktopApplication* app = DesktopApplication::GetInstance();
				Data* data = GetData(app);
				Window* window = data->main_window;

				if (!window->base->fullscreen)
					return;

				[window->object goWindow];

				const NSRect viewRectPoints = [window->view frame];
				const NSRect viewRectPixels = [window->view convertRectToBacking:viewRectPoints];

				window->base->width = viewRectPixels.size.width;
				window->base->height = viewRectPixels.size.height;
				window->base->aspect_ratio = static_cast<float>(window->base->width) / static_cast<float>(window->base->height);
				window->base->fullscreen = false;

				WindowController* window_controller = app->GetWindowController();
				if (window_controller)
					window_controller->OnResize(window->base->width, window->base->height);
			}
			void Center()
			{
				Window* main_window = ::GetMainWindow();

				// No need to center fullscreen window
				if (main_window->base->fullscreen)
					return;

				ScytheWindow * window = (ScytheWindow *)main_window->object; // to surpass -Wobjc-multiple-method-names
				[window center];
			}
			void Resize(int width, int height)
			{
				Window* window = ::GetMainWindow();

				NSRect viewRect;
				viewRect.origin.x = 0;
				viewRect.origin.y = 0;
				viewRect.size.width = width;
				viewRect.size.height = height;
				NSRect rect = [window->view convertRectToBacking:viewRect];
				[window->object setFrame:rect display:YES animate:NO];
			}
			void Iconify()
			{
				Window* window = ::GetMainWindow();
				[window->object miniaturize:nil];
			}
			void Restore()
			{
				Window* window = ::GetMainWindow();
				[window->object deminiaturize:nil];
			}
			void Show()
			{
				Window* window = ::GetMainWindow();
				[window->object makeKeyAndOrderFront:nil];
				window->base->visible = true;
			}
			void Hide()
			{
				Window* window = ::GetMainWindow();
				[window->object orderOut:nil];
				window->base->visible = false;
			}
			bool IsVisible()
			{
				Window* window = ::GetMainWindow();
				return window->base->visible;
			}
			void SetTitle(const wchar_t* title)
			{
				Window* window = ::GetMainWindow();
				NSUInteger byteLength = wcslen(title) * sizeof(wchar_t);
				NSString *titleStr = [[NSString alloc] initWithBytes:title
				                                              length:byteLength
				                                            encoding:NSUTF32LittleEndianStringEncoding];
				[window->object setTitle:titleStr];
				[titleStr release];
			}
			std::wstring GetTitle()
			{
				Window* window = ::GetMainWindow();
				NSString *title = [window->object title];
				if (!title || [title length] == 0)
					return std::wstring();
				NSUInteger byteLength = [title lengthOfBytesUsingEncoding:NSUTF32LittleEndianStringEncoding];
				std::wstring wstr;
    			wstr.resize(byteLength / sizeof(wchar_t));
    			[title getBytes:&wstr[0]
			          maxLength:byteLength
			         usedLength:nullptr
			           encoding:NSUTF32LittleEndianStringEncoding
			            options:0
			              range:NSMakeRange(0, [title length])
			     remainingRange:nullptr];
    			return wstr;
			}

		} // namespace window

		// Cursor operations
		namespace cursor {

			void GetPos(float* x, float* y)
			{
				Window* window = ::GetMainWindow();
				const NSPoint pos = [window->object mouseLocationOutsideOfEventStream];
				if (x)
					*x = static_cast<float>(pos.x);
				if (y)
					*y = static_cast<float>(pos.y);
			}
			void SetPos(float x, float y)
			{
				Window* window = ::GetMainWindow();
				const NSRect globalRect = [window->object convertRectToScreen:NSMakeRect(x, y, 0, 0)];
				const float displayHeight = CGDisplayBounds(CGMainDisplayID()).size.height;
				CGWarpMouseCursorPosition(CGPointMake(globalRect.origin.x, displayHeight - globalRect.origin.y));
			}
			void Center()
			{
				Window* window = ::GetMainWindow();
				const NSRect frameRect = [window->object frame];
				NSRect contentRect = [window->object contentRectForFrameRect:frameRect];
				contentRect.origin.x = contentRect.size.width/2;
				contentRect.origin.y = contentRect.size.height/2;
				const NSRect globalRect = [window->object convertRectToScreen:contentRect];
				const float displayHeight = CGDisplayBounds(CGMainDisplayID()).size.height;
				CGWarpMouseCursorPosition(CGPointMake(globalRect.origin.x, displayHeight - globalRect.origin.y));
			}
			void Show()
			{
				[NSCursor unhide];
			}
			void Hide()
			{
				[NSCursor hide];
			}

		} // namespace cursor

		// Clipboard operations
		namespace clipboard {

			void SetText(const char *text)
			{
				const NSPasteboardType kPasteboardType = 
			#if __MAC_OS_X_VERSION_MIN_REQUIRED < 101400
					NSStringPboardType;
			#else
					NSPasteboardTypeString;
			#endif
				NSArray* types = [NSArray arrayWithObjects:kPasteboardType, nil];
				
				NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
				[pasteboard declareTypes:types owner:nil];
				[pasteboard setString:[NSString stringWithUTF8String:text]
							  forType:kPasteboardType];
			}
			std::string GetText()
			{
				const NSPasteboardType kPasteboardType = 
			#if __MAC_OS_X_VERSION_MIN_REQUIRED < 101400
					NSStringPboardType;
			#else
					NSPasteboardTypeString;
			#endif
				NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
				
				if (![[pasteboard types] containsObject:kPasteboardType])
				{
					NSLog(@"Cocoa: Failed to retrieve string from pasteboard");
					return NULL;
				}
				
				NSString* object = [pasteboard stringForType:kPasteboardType];
				if (!object)
				{
					NSLog(@"Cocoa: Failed to retrieve object from pasteboard");
					return NULL;
				}
				
				std::string string = [object UTF8String];
				
				return string;
			}

		} // namespace clipboard

	} // namespace platform
} // namespace scythe