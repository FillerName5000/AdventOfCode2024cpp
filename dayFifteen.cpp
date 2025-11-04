#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace DayFifteen
{
	// [0,0]
	//       [xLen-1,yLen-1]
	namespace {
#pragma region charDef
		namespace GridChar {
			constexpr char Wall = '#';
			constexpr char Empty = '.';
			constexpr char Box = 'O';
			constexpr char Bot = '@';
			constexpr char Up = '^';
			constexpr char Right = '>';
			constexpr char Down = 'v';
			constexpr char Left = '<';
		}
#pragma endregion
		using coord_t = std::pair<long, long>;
		using char_square_t = std::vector<std::vector<char>>; // heap, too large for the stack
		enum Direction { Up, Right, Down, Left };
		void initializeGrid(const long kLen);
		void handleLine(std::string line);
		void coutCharSquareAsGrid(const std::string& prefix);
		void parseStartingStateLine(const std::string& line, const long kLen, long& currentRow);
		coord_t getNextCoord(const char gridChar, const coord_t& pos);
		bool isValidCoord(const long kLen, const coord_t& coord);
		Direction getDirectionFromAToB(const coord_t& a, const coord_t& b);
		void updateBotPos(const coord_t& newPos);
		char directionToChar(Direction dir);
		long calcTotal(const long kLen);

		const bool toLog = false; // true   false

		const int kMaxIter = 100'000;
		const long kLenTest = 10;
		const long kLenFull = 50;

		char_square_t grid;
		coord_t botPos{ -1, -1 };

		void moveBoxes(const Direction boxDir, const std::pair<coord_t,coord_t> startAndEndBox)
		{
			grid[startAndEndBox.first.first][startAndEndBox.first.second] = GridChar::Empty;
			grid[startAndEndBox.second.first][startAndEndBox.second.second] = GridChar::Box;

			grid[botPos.first][botPos.second] = GridChar::Empty;
			botPos = startAndEndBox.first;
			grid[botPos.first][botPos.second] = GridChar::Bot;
			if (toLog) coutCharSquareAsGrid("state: \n");
		}

		void handleBoxMovement(const coord_t firstBoxPos, const Direction boxDir, const long kLen)
		{
			std::pair<coord_t, coord_t> startAndEndBox;
			startAndEndBox.first = firstBoxPos;
			coord_t currentPos = firstBoxPos;
			char dirChar = directionToChar(boxDir);
			bool endCharIsEmpty = false;

			int iter = 0;
			while (isValidCoord(kLen, currentPos) || iter > kMaxIter) {
				++iter;
				char currentChar = grid[currentPos.first][currentPos.second];

				if (currentChar == GridChar::Wall || currentChar == GridChar::Empty) {
					endCharIsEmpty = (currentChar == GridChar::Empty);
					startAndEndBox.second = currentPos;
					break;
				}

				currentPos = getNextCoord(dirChar, currentPos);
			}
			if (endCharIsEmpty) {
				moveBoxes(boxDir, startAndEndBox);
			}
			else {
				if (toLog) std::cout << "Wall end found at [" << currentPos.first << "," << currentPos.second << "]\n";
			}
		}

		void handleDirections(const std::string& directions, const long kLen)
		{
			if (toLog) std::cout << directions << '\n';
			for (const char dir : directions)
			{
				if (toLog) std::cout << dir << '\n';
				coord_t nextBotPos = getNextCoord(dir, botPos);
				if (!isValidCoord(kLen, nextBotPos)) throw std::runtime_error("Bot wants to escape wall containment");

				char nextPosChar = grid[nextBotPos.first][nextBotPos.second];
				switch (nextPosChar) {
				case GridChar::Wall:
					if (toLog) std::cout << "Wall at [" << nextBotPos.first << "," << nextBotPos.second << "]\n";
					continue;
				case GridChar::Box:
					handleBoxMovement(nextBotPos, getDirectionFromAToB(botPos, nextBotPos), kLen);
					continue;
				case GridChar::Empty:
					updateBotPos(nextBotPos);
					continue;
				}
			}
		}

		void handleFile(std::ifstream& inputFile, const long kLen)
		{
			if (inputFile.is_open()) {
				initializeGrid(kLen); // & botpos init too

				std::string line;
				bool afterBlankLine = false;
				long currentRow = 0;

				while (getline(inputFile, line)) {
					if (line.empty()) {
						afterBlankLine = true;
						continue;
					}

					if (!afterBlankLine) {
						parseStartingStateLine(line, kLen, currentRow);
					}
					else {
						if (toLog) coutCharSquareAsGrid("Start: \n");
						handleDirections(line, kLen); // 20 000 line length full
						if (toLog) coutCharSquareAsGrid("End: \n");
					}
				}

				std::cout << "Finished reading file\n";

				inputFile.close(); // automatically happens when going out of scope but no longer needed. More about explicitness.

				std::cout << "Total: " << calcTotal(kLen) << '\n';

				std::cout << "\nFinished running program\n";
			}
			else {
				std::cout << "Unable to open file\n";
			}
		}

		long calcTotal(const long kLen)
		{
			const long gpsMult = 100;
			long total = 0;

			for (long y = 0; y < kLen; ++y) {
				for (long x = 0; x < kLen; ++x) {
					if (grid[y][x] == GridChar::Box) {
						if (toLog) std::cout << "Box at [" << y << "," << x << "] adds " << (y * gpsMult) + x << '\n';
						total += (y * gpsMult) + x;
					}
				}
			}

			return total;
		}


#pragma region helper
		void updateBotPos(const coord_t& newPos) {
			grid[botPos.first][botPos.second] = GridChar::Empty;
			grid[newPos.first][newPos.second] = GridChar::Bot;
			botPos = newPos;
			if (toLog) coutCharSquareAsGrid("++E: State: \n");
		}
		char directionToChar(Direction dir) {
			switch (dir) {
			case Direction::Up:    return GridChar::Up;
			case Direction::Right: return GridChar::Right;
			case Direction::Down:  return GridChar::Down;
			case Direction::Left:  return GridChar::Left;
			}
			std::cout << dir;
			throw std::runtime_error("Invalid dir passed");
		}
		Direction getDirectionFromAToB(const coord_t& a, const coord_t& b) {
			long dx = b.second - a.second;
			long dy = b.first- a.first;

			if (abs(dx) > abs(dy)) {
				return dx > 0 ? Direction::Right : Direction::Left;
			}
			else {
				return dy > 0 ? Direction::Down : Direction::Up;
			}
		}
		bool isValidCoord(const long kLen, const coord_t& coord) {
			return coord.first >= 0 && coord.first < kLen && coord.second >= 0 && coord.second < kLen;
		}
		coord_t getNextCoord(const char gridChar, const coord_t& pos) {
			coord_t newPos = pos;
			switch (gridChar) {
			case GridChar::Up:    newPos.first--; break;
			case GridChar::Right: newPos.second++;  break;
			case GridChar::Down:  newPos.first++; break;
			case GridChar::Left:  newPos.second--;  break;
			}
			return newPos;
		}
#pragma endregion

#pragma region cout
		void coutCharSquareAsGrid(const std::string& prefix) {
			std::cout << prefix;
			for (const auto& row : grid) {
				for (const auto& c : row) {
					std::cout << c;
				}
				std::cout << '\n';
			}
			std::cout << '\n';
		}
#pragma endregion

#pragma region startState
		void parseStartingStateLine(const std::string& line, const long kLen, long& currentRow)
		{
			if (grid.empty() || currentRow >= grid.size()) {
				return;
			}

			for (long x = 0; x < kLen; ++x) {
				grid[currentRow][x] = line[x];
				if (line[x] == GridChar::Bot) {
					botPos = { x, currentRow };
				}
			}

			++currentRow;
		}

		void initializeGrid(const long kLen) {
			grid.resize(kLen, std::vector<char>(kLen, GridChar::Empty));
		}
#pragma endregion
	}
	void dayFifteen() {
		std::system("cls"); // clear terminal pre-boot
		std::cout << "Running program DayFifteen" << '\n';
		const bool isFullFile = true; // true   false
		const long kLen = isFullFile ? kLenFull : kLenTest;

		std::string line;
		std::ifstream inputFile;
		if (isFullFile) {
			std::cout << "Using full file\n\n";
		}
		else {
			std::cout << "Using test file\n\n";
		}
		(isFullFile) ? inputFile.open("dayFifteenFull.txt") : inputFile.open("dayFifteenTest.txt");
		handleFile(inputFile, kLen);
	}
}
