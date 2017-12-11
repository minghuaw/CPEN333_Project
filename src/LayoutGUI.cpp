#include "Layout.h"
#include "Amazoom.h"

#include <cpen333/process/shared_memory.h>
#include <cpen333/process/mutex.h>
#include <cpen333/console.h>
#include <cstdio>
#include <thread>
#include <chrono>
#include <fstream>
#include <string>
#include <random>
#include <ctime>

/**
* Handles all drawing/memory synchronization for the
* User Interface process
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

public:
	LayoutUI() : display_(), memory_(WAREHOUSE_MEMORY_NAME), mutex_(WAREHOUSE_MEMORY_MUTEX_NAME) {

		// clear display and hide cursor
		display_.clear_all();
		display_.set_cursor_visible(false);

		for (size_t i = 0; i<MAX_ROBOTS; ++i) {
			lastpos_[i][COL_IDX] = -1;
			lastpos_[i][ROW_IDX] = -1;
		}
	}

	/**
	* Draws the maze itself
	*/
	void draw_maze() {
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
					std::printf("%c", 'X');
				}
				else if (ch == SHELF_CHAR) {
					std::printf("%c", '|');
				}
				else if (ch == LOADINGBAY_CHAR) {
					std::printf("%c", '_');
				}
				else if (ch == UNLOADINGBAY_CHAR) {
					std::printf("%c", '_');
				}
				else {
					std::printf("%c", EMPTY_CHAR);
				}
			}
		}
	}

	/**
	* Draws all robots in the warehouse
	*/
	void draw_robots() {

		RobotInfo& rinfo = memory_->rinfo;

		// draw all runner locations
		int nrunners;
		{
			std::lock_guard<decltype(mutex_)> lock(mutex_);
			nrunners = rinfo.nrobots;
		}
		for (size_t i = 0; i < nrunners; ++i) {
			char me = 'A' + i;
			mutex_.lock();
			int newr = rinfo.rloc[i][ROW_IDX];
			int newc = rinfo.rloc[i][COL_IDX];
			mutex_.unlock();

			
			if (newc != lastpos_[i][COL_IDX] || newr != lastpos_[i][ROW_IDX]) {
				// zero out last spot and update known location
				display_.set_cursor_position(YOFF + lastpos_[i][ROW_IDX], XOFF + lastpos_[i][COL_IDX]);
				std::printf("%c", EMPTY_CHAR);
				lastpos_[i][COL_IDX] = newc;
				lastpos_[i][ROW_IDX] = newr;
				display_.set_cursor_position(YOFF + newr, XOFF + newc);
				std::printf("%c", me);
			}
			else if (newc == lastpos_[i][COL_IDX] && newr == lastpos_[i][ROW_IDX]) {
				display_.set_cursor_position(YOFF + lastpos_[i][ROW_IDX], XOFF + lastpos_[i][COL_IDX]);
				std::printf("%c", me);
			}
			else if (newc == -1 && newr == -1) {
				display_.set_cursor_position(YOFF + lastpos_[i][ROW_IDX], XOFF + lastpos_[i][COL_IDX]);
				std::printf("%c", EMPTY_CHAR);
			}
		}
	}
			// TODO: verify if this needs to be uncommented
			//// if not already at the exit...
			//if (newc != loading_[COL_IDX] || newr != loading_[ROW_IDX]) {
			//	if (newc != lastpos_[i][COL_IDX]
			//		|| newr != lastpos_[i][ROW_IDX]) {

			//		// zero out last spot and update known location
			//		display_.set_cursor_position(YOFF + lastpos_[i][ROW_IDX], XOFF + lastpos_[i][COL_IDX]);
			//		std::printf("%c", EMPTY_CHAR);
			//		lastpos_[i][COL_IDX] = newc;
			//		lastpos_[i][ROW_IDX] = newr;
			//	}

			//	// print runner at new location
			//	display_.set_cursor_position(YOFF + newr, XOFF + newc);
			//	std::printf("%c", me);
			//}
			//else {

			//	// erase old position if now finished
			//	if (lastpos_[i][COL_IDX] != loading_[COL_IDX] || lastpos_[i][ROW_IDX] != loading_[ROW_IDX]) {
			//		display_.set_cursor_position(YOFF + lastpos_[i][ROW_IDX], XOFF + lastpos_[i][COL_IDX]);
			//		std::printf("%c", EMPTY_CHAR);
			//		lastpos_[i][COL_IDX] = newc;
			//		lastpos_[i][ROW_IDX] = newr;

			//		// display a completion message
			//		display_.set_cursor_position(YOFF, XOFF + memory_->linfo.cols + 2);
			//		std::printf("runner %c escaped!!", me);
			//	}
			//}

	/**
	* draw status of all trucks
	*/
	void draw_trucks() {
		TruckInfo& tinfo = memory_->tinfo;

		int ndtrucks, nrtrucks, print_count = 0, colwidth;
		{
			std::lock_guard<decltype(mutex_)> lock(mutex_);
			ndtrucks = tinfo.ndtrucks;
			nrtrucks = tinfo.nrtrucks;
			colwidth = memory_->linfo.cols;
		}
		for (size_t i = 0; i < ndtrucks; ++i) {
			char me = 'A' + i;
			char s; // status of trucks
			double cap;
			double weight;
			mutex_.lock();
			s = tinfo.dtruckStatus[i];
			cap = tinfo.dcapcity[i];
			weight = tinfo.dweight[i];
			mutex_.unlock();
			// set print location and increase count
			display_.set_cursor_position(YOFF + print_count, XOFF + colwidth + 2);
			print_count++;
			std::printf("Delivery Truck %c: status %c, capcity %.2f kg, current load %.2f kg", me, s, cap, weight);
		}
		for (size_t i = 0; i < nrtrucks; ++i) {
			char me = 'A' + i;
			char s; // status of trucks
			double cap;
			double weight;
			mutex_.lock();
			s = tinfo.rtruckStatus[i];
			cap = tinfo.rcapcity[i];
			weight = tinfo.rweight[i];
			mutex_.unlock();
			// set print location and increase count
			display_.set_cursor_position(YOFF + print_count, XOFF + colwidth + 2);
			print_count++;
			std::printf("Restock Truck %c: status %c, capcity %.2f kg, current load %.2f kg", me, s, cap, weight);
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
	cpen333::process::shared_object<SharedData> memory(WAREHOUSE_MEMORY_NAME);
	cpen333::process::mutex mutex(WAREHOUSE_MEMORY_MUTEX_NAME);
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
			ui.draw_robots();
			ui.draw_trucks();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
	else {
		std::cout << "main not initialized" << std::endl;
		std::cin.get();
	}
	return 0;
}