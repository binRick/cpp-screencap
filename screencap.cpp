#include "ScreenCapture.h"
#include "ScreenCapture_C_API.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <climits>
#include <iostream>
#include <locale>
#include <string>
#include <thread>
#include <vector>
#include <jansson.h>


#define MSF_GIF_IMPL
#define MSF_USE_ALPHA
#include "include/msf_gif.h"
/////////////////////////////////////////////////////////////////////////
#define CAPTURE_INTERVAL 2000
#define CONVERT_IMAGES true
#define WRITE_ANIMATED_GIFS true
#define CAPTURE_SECONDS 5
#define DEBUG_MODE true
#define MAX_MONITORS 32

extern int msf_gif_bgra_flag;

void jansson_dev(void){
    char text[4096] = "{\"abc\": 123}";

    json_t *root;
    json_error_t error;

    root = json_loads(text, 0, &error);

}

void ExtractAndConvertToRGBA(const SL::Screen_Capture::Image &img, unsigned char *dst, size_t dst_size)
{
    assert(dst_size >= static_cast<size_t>(SL::Screen_Capture::Width(img) * SL::Screen_Capture::Height(img) * sizeof(SL::Screen_Capture::ImageBGRA)));
    auto imgsrc = StartSrc(img);
    auto imgdist = dst;
    for (auto h = 0; h < Height(img); h++) {
        auto startimgsrc = imgsrc;
        for (auto w = 0; w < Width(img); w++) {
            *imgdist++ = imgsrc->R;
            *imgdist++ = imgsrc->G;
            *imgdist++ = imgsrc->B;
            *imgdist++ = 0; // alpha should be zero
            imgsrc++;
        }
        imgsrc = SL::Screen_Capture::GotoNextRow(img, startimgsrc);
    }
}

using namespace std::chrono_literals;
std::shared_ptr<SL::Screen_Capture::IScreenCaptureManager> framgrabber;
std::atomic<int> realcounter;
std::atomic<int> onNewFramecounter;

inline std::ostream &operator<<(std::ostream &os, const SL::Screen_Capture::ImageRect &p)
{
    return os << "left=" << p.left << " top=" << p.top << " right=" << p.right << " bottom=" << p.bottom;
}
inline std::ostream &operator<<(std::ostream &os, const SL::Screen_Capture::Monitor &p)
{
    return os << "Id=" << p.Id << " Index=" << p.Index << " Height=" << p.Height << " Width=" << p.Width << " OffsetX=" << p.OffsetX
              << " OffsetY=" << p.OffsetY << " Name=" << p.Name;
}
auto onNewFramestart = std::chrono::high_resolution_clock::now();
MsfGifState gifStates[MAX_MONITORS] = {};
bool GIF_STARTED = false;
int gif_frames_qty[MAX_MONITORS] = {};
auto last_frame_time = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
std::vector<long> monitor_last_frame_times(MAX_MONITORS);


void createframegrabber(){
    realcounter = 0;
    onNewFramecounter = 0;
    framgrabber = nullptr;
    framgrabber =
        SL::Screen_Capture::CreateCaptureConfiguration([]() {
            auto mons = SL::Screen_Capture::GetMonitors();
	    if(DEBUG_MODE){
              std::cout << "Library is requesting the list of monitors to capture!" << std::endl;
              for (auto &m : mons) {
                std::cout << m << std::endl;
              }
	    }
            return mons;
        })->onNewFrame([&](const SL::Screen_Capture::Image &img, const SL::Screen_Capture::Monitor &monitor) {
                 auto r = realcounter.fetch_add(1);
		 bool capture_frame = false;

		 if(monitor_last_frame_times.at(monitor.Index) == 0){
			capture_frame = true;
		 }else{
			if((std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count() - monitor_last_frame_times.at(monitor.Index)) >= CAPTURE_INTERVAL)
				capture_frame = true;
		 }

		 if(capture_frame){
    			monitor_last_frame_times.at(monitor.Index)=std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
		 	last_frame_time = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
			if(CONVERT_IMAGES){
				auto size = Width(img) * Height(img) * sizeof(SL::Screen_Capture::ImageBGRA);
				auto imgbuffer(std::make_unique<unsigned char[]>(size));
				ExtractAndConvertToRGBA(img, imgbuffer.get(), size);
				if(WRITE_ANIMATED_GIFS){
				  if(DEBUG_MODE){
					auto ds = "Writing Frames to animated gif :: monitor #"
						+ std::to_string(monitor.Index)
					;
					std::cout << ds << std::endl;
				  }
				  msf_gif_frame(&(gifStates[monitor.Index]), (unsigned char *)imgbuffer.get(), (int)(CAPTURE_INTERVAL/10), (int)4, (int)(Width(img) * 4));
				  gif_frames_qty[monitor.Index]++;
				}
			}
		}

                if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - onNewFramestart).count() >= 1000) {
                    onNewFramecounter = 0;
                    onNewFramestart = std::chrono::high_resolution_clock::now();
                }
                onNewFramecounter += 1;
            })->start_capturing();

}



int main(){
    std::srand(std::time(nullptr));

    std::cout << "Checking for Permission to capture the screen" << std::endl;
    if (SL::Screen_Capture::IsScreenCaptureEnabled()) {
        std::cout << "Application Allowed to Capture the screen!" << std::endl;
    }
    else if (SL::Screen_Capture::CanRequestScreenCapture()) {
        std::cout << "Application Not Allowed to Capture the screen. Waiting for permission " << std::endl;
        while (!SL::Screen_Capture::IsScreenCaptureEnabled()) {
            SL::Screen_Capture::RequestScreenCapture();
            std::cout << " . ";
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    else {
        std::cout << "Cannot Capture the screen due to permission issues. Exiting" << std::endl;
        return 0;
    }

    msf_gif_bgra_flag = false;
    auto goodmonitors = SL::Screen_Capture::GetMonitors();
    for (auto &m : goodmonitors) {
        std::cout << m << std::endl;
        assert(SL::Screen_Capture::isMonitorInsideBounds(goodmonitors, m));
	gifStates[m.Index] = {};
    	msf_gif_begin(&(gifStates[m.Index]), m.Width, m.Height);
    }



    std::cout << "Running display capturing for " + std::to_string(CAPTURE_SECONDS) + " seconds" << std::endl;
    createframegrabber();
    std::this_thread::sleep_for(std::chrono::seconds(CAPTURE_SECONDS));
    framgrabber->pause();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    for (auto &m : goodmonitors) {
        std::cout << m << std::endl;
        assert(SL::Screen_Capture::isMonitorInsideBounds(goodmonitors, m));
	if(gif_frames_qty[m.Index] > 0){
		auto gif_file = "monitor_" + std::to_string(m.Index) + ".gif";
		auto s = "Writing gif for monitor #" 
			+ std::to_string(m.Index) 
			+ " " + gif_file + " with #"
			+ std::to_string(gif_frames_qty[m.Index]) + " frames"
		;
		std::cout << s << std::endl;
		MsfGifResult result = msf_gif_end(&gifStates[m.Index]);
		if (result.dataSize > 0) {
		  std::cout << "opening" << std::endl;
		  FILE *fp = std::fopen(gif_file.c_str(), "wb");
		  std::cout << "opened" << std::endl;
		  if(fp){
			  fwrite(result.data, result.dataSize, 1, fp);
			  fclose(fp);
		  }
		}
		msf_gif_free(result);
		s = "Wrote File";
		std::cout << s << std::endl;
		
	}
    }
    //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    return 0;
}
