#ifndef __SCYTHE_MAIN_H__
#define __SCYTHE_MAIN_H__

#include "application.h"
#include "platform.h"
#include "defines.h"

/**
 * @brief      The macro to generate main function body.
 *
 * @param      U     The user application class.
 * @param      N     The number of command line arguments (argc).
 * @param      V     The pointer to array of command line arguments (argv).
 */
#define SCYTHE_MACRO_MAIN_BODY(U, N, V)														\
{																							\
static_assert(std::is_convertible<U*, scythe::Application*>::value, "Inheritance issue");	\
scythe::Application::CreateInstance<U>();													\
int r = scythe::Application::GetInstance()->Run(N,V);										\
scythe::Application::DestroyInstance();														\
return r;																					\
}

/**
 * Main function signature definition
 */
#if defined(SCYTHE_TARGET_WINDOWS) && defined(SCYTHE_WINDOWS_NO_CONSOLE)

# include "platform_includes.h"

# define SCYTHE_MACRO_MAIN_SIGNATURE														\
int WINAPI WinMain(HINSTANCE hInstance,														\
				   HINSTANCE hPrevInstance,													\
				   LPSTR lpCmdLine,															\
				   int nCmdShow)
# define SCYTHE_MACRO_MAIN_ARGC 0
# define SCYTHE_MACRO_MAIN_ARGV nullptr

#else

# define SCYTHE_MACRO_MAIN_SIGNATURE														\
int main(int argc, const char ** argv)
# define SCYTHE_MACRO_MAIN_ARGC argc
# define SCYTHE_MACRO_MAIN_ARGV argv

#endif

/**
 * @brief      Main function definition macro
 *
 * @param      U     The user application class
 */
#define SCYTHE_DECLARE_MAIN(U) 																\
SCYTHE_MACRO_MAIN_SIGNATURE																	\
SCYTHE_MACRO_MAIN_BODY(U, SCYTHE_MACRO_MAIN_ARGC, SCYTHE_MACRO_MAIN_ARGV)

#endif