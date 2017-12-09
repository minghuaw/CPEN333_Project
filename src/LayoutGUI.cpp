#include "Layout.h"
#include "Amazoom.h"

#include <cpen333/process/shared_memory.h>
#include <cpen333/process/mutex.h>
#include <cpen333/console.h>
#include <cstdio>
#include <thread>
#include <chrono>

/**
* Handles all drawing/memory synchronization for the
* User Interface process
*
*/
// display offset for better visibility
class LayoutUI {
	static const int XOFF = 2;
	static const int YOFF = 1;

	cpen333::console display_;
	cpen333::process::shared_object<SharedData> memory_;
	cpen333::process::mutex mutex_;

	// previous positions of runners
	int lastpos_[MAX_ROBOTS][2];
	int exit_[2];   // exit location

public:
	LayoutUI() : display_(), memory_(LAYOUT_MEMORY_NAME), mutex_(LAYOUT_MEMORY_MUTEX_NAME) {

		// clear display and hide cursor
		display_.clear_all();
		display_.set_cursor_visible(false);

		// initialize last known runner positions
		for (size_t i = 0; i<MAX_ROBOTS; ++i) {
			lastpos_[i][COL_IDX] = -1;
			lastpos_[i][ROW_IDX] = -1;
		}

		// initialize exit location
		exit_[COL_IDX] = -1;
		exit_[ROW_IDX] = -1;

		//===========================================================
		// TODO: SEARCH MAZE FOR EXIT LOCATION
		//===========================================================
		LayoutInfo& linfo = memory_->linfo;
		std::lock_guard<decltype(mutex_)> lock(mutex_);
		for (int r = 0; r < linfo.rows && exit_[0] < 0; ++r) {
			for (int c = 0; c < linfo.cols; ++c) {
				char ch = linfo.layout[c][r];
				if (ch == EXIT_CHAR) {
					exit_[0] = c;
					exit_[1] = r;
					break;
				}
			}
		}
	}

	/**
	* Draws the maze itself
	*/
	void draw_maze() {
		static const char WALL = 219;  // WALL character
		static const char EXIT = 176;  // EXIT character

		LayoutInfo& linfo = memory_->linfo;
		RobotInfo& rinfo = memory_->rinfo;

		// clear display
		display_.clear_display();

		// draw maze
		for (int r = 0; r < linfo.rows; ++r) {
			display_.set_cursor_position(YOFF + r, XOFF);
			for (int c = 0; c < linfo.cols; ++c) {
				mutex_.lock();
				char ch = linfo.layout[c][r];
				mutex_.unlock();
				if (ch == WALL_CHAR) {
					std::printf("%c", '|');
				}
				else if (ch == EXIT_CHAR) {
					std::printf("%c", 'O');
				}
				else {
					std::printf("%c", EMPTY_CHAR);
				}
			}
		}
	}

	/**
	* Draws all runners in the maze
	*/
	void draw_runners() {

		RobotInfo& rinfo = memory_->rinfo;

		// draw all runner locations
		int nrunners;
		{
			std::lock_guard<decltype(mutex_)> lock(mutex_);
			nrunners = rinfo.nrobots;
		}
		for (size_t i = 0; i<nrunners; ++i) {
			char me = 'A' + i;
			mutex_.lock();
			int newr = rinfo.rloc[i][ROW_IDX];
			int newc = rinfo.rloc[i][COL_IDX];
			mutex_.unlock();

			// if not already at the exit...
			if (newc != exit_[COL_IDX] || newr != exit_[ROW_IDX]) {
				if (newc != lastpos_[i][COL_IDX]
					|| newr != lastpos_[i][ROW_IDX]) {

					// zero out last spot and update known location
					display_.set_cursor_position(YOFF + lastpos_[i][ROW_IDX], XOFF + lastpos_[i][COL_IDX]);
					std::printf("%c", EMPTY_CHAR);
					lastpos_[i][COL_IDX] = newc;
					lastpos_[i][ROW_IDX] = newr;
				}

				// print runner at new location
				display_.set_cursor_position(YOFF + newr, XOFF + newc);
				std::printf("%c", me);
			}
			else {

				// erase old position if now finished
				if (lastpos_[i][COL_IDX] != exit_[COL_IDX] || lastpos_[i][ROW_IDX] != exit_[ROW_IDX]) {
					display_.set_cursor_position(YOFF + lastpos_[i][ROW_IDX], XOFF + lastpos_[i][COL_IDX]);
					std::printf("%c", EMPTY_CHAR);
					lastpos_[i][COL_IDX] = newc;
					lastpos_[i][ROW_IDX] = newr;

					// display a completion message
					display_.set_cursor_position(YOFF, XOFF + memory_->linfo.cols + 2);
					std::printf("runner %c escaped!!", me);
				}
			}
		}
	}

	/**
	* Checks if we are supposed to quit
	* @return true if memory tells us to quit
	*/
	bool quit() {
		// check if we need to quit
		std::lock_guard<decltype(mutex_)> lock(mutex_);
		return memory_->quit;
	}

	~LayoutUI() {
		// reset console settings
		display_.clear_all();
		display_.reset();
	}
};

int main() {
	// check if main has started
	cpen333::process::shared_object<SharedData> memory(LAYOUT_MEMORY_MUTEX_NAME);
	cpen333::process::mutex mutex(LAYOUT_MEMORY_MUTEX_NAME);
	int magic;
	{
		std::unique_lock<decltype(mutex)> lock(mutex);
		magic = memory->magic;
	}
	if (magic == LAYOUT_MAGIC_KEY) {
		// initialize previous locations of characters
		LayoutUI ui;
		ui.draw_maze();

		// continue looping until main program has quit
		while (!ui.quit()) {
			ui.draw_runners();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
	else {
		std::cout << "main not initialized" << std::endl;
		std::cin.get();
	}
	return 0;
}