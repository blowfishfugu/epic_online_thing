#ifdef _WIN32
//have been shocked, wglGetProc("glBufferData") failed,
//then remembered that integrated graphic has incomplete gl-standards
//for notebooks, choose graphicscard instead of embedded intel-graphics.
//nv and amd look for theese exported flags
//deepdive documentation:
// https://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
extern "C" {
	_declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

#endif