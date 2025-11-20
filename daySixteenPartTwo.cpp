#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <limits>
#include <tuple>
#include <queue>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h> // just for fancy logging with colors in the debug window
#endif

namespace DaySixteenPartTwo
{
#pragma region charDef
	namespace GridChar {
		constexpr char Wall = '#';
		constexpr char Empty = '.';
		constexpr char Start = 'S';
		constexpr char End = 'E';
	}
#pragma endregion
	namespace {
#pragma region baseDefs
		struct PairHash {
			template <typename T1, typename T2>
			std::size_t operator()(const std::pair<T1, T2>& pair) const {
				return std::hash<T1>()(pair.first) ^ (std::hash<T2>()(pair.second) << 1);
			}
		};
		enum Direction { North, East, South, West };
		using coord_t = std::pair<size_t, size_t>;
		using coordn_dir_t = std::pair<coord_t, Direction>;
		struct VectorHash {
			std::size_t operator()(const std::vector<coordn_dir_t>& vec) const {
				std::size_t hashValue = 0;
				for (const auto& elem : vec) {
					std::size_t coordHash = PairHash()(elem.first);
					std::size_t dirHash = std::hash<int>()(static_cast<int>(elem.second));
					hashValue ^= coordHash ^ (dirHash << 1);
				}
				return hashValue;
			}
		};
		using char_square_t = std::vector<std::vector<char>>;
		void initializeGrid(const size_t kLen);
		void fillLineInVector(const std::string& line, const size_t kLen, int currentRow);
#pragma endregion
		using sequences_t = std::unordered_set<std::vector<coordn_dir_t>, VectorHash>;
		using state_t = std::tuple<long, coord_t, Direction, std::vector<coordn_dir_t>>;
		struct CompareStates {
			bool operator()(const state_t& stateA, const state_t& stateB) const {
				return std::get<0>(stateA) > std::get<0>(stateB); // B state must be smaller as true invokes a move up for the new element
				// from docs: using std::greater<T> would cause the smallest element to appear as the top(). 
			}
		};
		struct RevState {
			long cost;
			coord_t coord;
			Direction dir;
			std::vector<coordn_dir_t> path;
		};
		struct RevCompareStates {
			bool operator()(const RevState& a, const RevState& b) const {
				return a.cost < b.cost;
			}
		};
		void coutGridWithPos(const coord_t& pos, const size_t kLen, const std::string prefix);
		void coutPathAsGrid(const std::vector<coordn_dir_t>& path, const size_t kLen);
		void coutCostToReachGrid(const std::unordered_map<coord_t, long, PairHash>& costToReach, const size_t kLen);
		void coutPathIndexGrid(const std::vector<coordn_dir_t>& path, const size_t kLen);

		bool isValidMove(const coord_t& pos, const size_t kLen);
		std::pair<coord_t, coord_t> findStartAndEndPositions(const size_t kLen);
		coord_t moveForward(const coord_t& pos, Direction dir);
		Direction turnRight(Direction dir);
		Direction turnLeft(Direction dir);

		const bool toLog = true; // true   false
		const int kMaxIter = 10'000'000; // Not enough for the iterative approach, time to use an actual path navigation algo (dijkstra)
		const int turnCost = 1000;
		const Direction startDirection = Direction::East;
		const long checkedPathCoordCost = 99'999'999; // 2.1B is long maxVal on my system. This value is high enough (possible max for the input should be seven-eight digits) 
		// to not interfere with the possible values, but the exact value is arbitrary. For non-leetcode applications, this is non-viable.

		char_square_t grid;

		void coutUniqueCoords(const std::unordered_set<coord_t, PairHash>& uniqueCoords) {
			std::cout << "\nRegistered Coords:\n";
			char_square_t gridCopy = grid;

			for (const auto& coord : uniqueCoords) {
				if (coord.first < gridCopy.size() && coord.second < gridCopy[coord.first].size()) {
					gridCopy[coord.first][coord.second] = 'K';
				}
			}

			for (const auto& row : gridCopy) {
				for (const auto& cell : row) {
					std::cout << cell;
				}
				std::cout << '\n';
			}
		}

		bool isMultiSplitCrossRoads(const coord_t& pos, const size_t kLen) {
			int validMoveCount = 0;
			for (size_t directionIndex = 0; directionIndex < 4; ++directionIndex) {
				Direction testDirection = static_cast<Direction>(directionIndex);
				coord_t testPosition = moveForward(pos, testDirection);
				if (isValidMove(testPosition, kLen)) {
					++validMoveCount;
				}
			}
			return validMoveCount >= 3;
		}

		bool arePathsIdentical(const std::vector<coordn_dir_t>& pathA, const std::vector<coordn_dir_t>& pathB) {
			size_t aSize = pathA.size();
			size_t bSize = pathB.size();
			if (aSize != bSize) {
				return false;
			}

			for (size_t i = 0; i < aSize; ++i) {
				if (pathA[i].first.first != pathB[i].first.first ||
					pathA[i].first.second != pathB[i].first.second ||
					pathA[i].second != pathB[i].second) {
					return false;
				}
			}

			return true;
		}

		size_t findCrossPathIndexInPath(const std::vector<coordn_dir_t>& optimalPath, coord_t crossCoord, Direction crossDir) {
			size_t s = optimalPath.size();
			for (size_t i = 0; i < s; ++i) {
				if (optimalPath[i].first == crossCoord && optimalPath[i].second == crossDir) {
					return i;
				}
			}
			return static_cast<size_t>(-1); // not found (unsigned)
		}

		bool findAlternativeReversePathAny(coord_t crossCoord, Direction crossDir, long crossCost, const std::unordered_set<coord_t, PairHash>& forbidden,
			const std::unordered_map<coord_t, long, PairHash>& costToReach, const std::vector<coordn_dir_t>& optimalPath, size_t crossPathIdx, const size_t kLen,
			coord_t& foundCoord, Direction& foundDir, long& foundCost)
		{
			std::priority_queue<RevState, std::vector<RevState>, RevCompareStates> revQueue;
			std::vector<coordn_dir_t> initialRevPath;
			initialRevPath.push_back(std::make_pair(crossCoord, crossDir));
			revQueue.push({ crossCost, crossCoord, crossDir, initialRevPath });

			std::unordered_map<coord_t, long, PairHash> reverseCostToReach;
			reverseCostToReach[crossCoord] = crossCost;

			while (!revQueue.empty()) {
				RevState state = revQueue.top();
				revQueue.pop();

				long curCost = state.cost;
				coord_t curCoord = state.coord;
				Direction curDir = state.dir;

				if (curCost < 1) {
					continue;
				}

				for (size_t i = 0; i < crossPathIdx; ++i) {
					if (optimalPath[i].first == curCoord && optimalPath[i].second == curDir) {
						std::unordered_map<coord_t, long, PairHash>::const_iterator costIt = costToReach.find(curCoord);
						if (costIt != costToReach.end() && costIt->second == curCost) {
							foundCoord = curCoord;
							foundDir = curDir;
							foundCost = curCost;
							return true;
						}
					}
				}

				for (int dirIdx = 0; dirIdx < 3; ++dirIdx) {
					Direction newDir = curDir;
					if (dirIdx == 1) newDir = turnLeft(curDir);
					else if (dirIdx == 2) newDir = turnRight(curDir);

					coord_t newCoord = moveForward(curCoord, newDir);

					if (!isValidMove(newCoord, kLen)) {
						continue;
					}
					if (forbidden.count(newCoord) > 0) {
						continue;
					}

					long newCost = curCost - 1;
					if (newDir != curDir) {
						newCost -= turnCost;
					}

					if (newCost < 1) {
						continue;
					}

					if (reverseCostToReach.find(newCoord) != reverseCostToReach.end() &&
						newCost <= reverseCostToReach[newCoord]) {
						continue;
					}

					std::unordered_map<coord_t, long, PairHash>::const_iterator costIt = costToReach.find(newCoord);
					if (costIt == costToReach.end() || costIt->second != newCost) {
						continue;
					}

					std::vector<coordn_dir_t> newPath = state.path;
					newPath.push_back(std::make_pair(newCoord, newDir));

					reverseCostToReach[newCoord] = newCost;
					revQueue.push({ newCost, newCoord, newDir, newPath });
				}
			}
			return false;
		}

		void crossroadsCalc(const std::vector<coordn_dir_t>& optimalPath, const std::vector<std::pair<coord_t, Direction>>& crossRoadsCoords,
			const std::unordered_map<coord_t, long, PairHash>& costToReach, const size_t kLen, std::unordered_set<coord_t, PairHash>& uniqueCoords)
		{
			size_t crSize = crossRoadsCoords.size();
			for (size_t crossIdx = 0; crossIdx < crSize; ++crossIdx) {
				coord_t crossCoord = crossRoadsCoords[crossIdx].first;
				Direction crossDir = crossRoadsCoords[crossIdx].second;

				size_t crossPathIdx = findCrossPathIndexInPath(optimalPath, crossCoord, crossDir);
				if (crossPathIdx == static_cast<size_t>(-1)) {
					throw std::runtime_error("Crossroads registered but not in optimal path");
				}

				long crossCost = costToReach.at(crossCoord);

				std::unordered_set<coord_t, PairHash> forbidden;
				for (size_t i = 0; i <= crossPathIdx; ++i) {
					forbidden.insert(optimalPath[i].first);
				}

				coord_t foundCoord;
				Direction foundDir;
				long foundCost;
				bool found = findAlternativeReversePathAny(
					crossCoord, crossDir, crossCost, forbidden,
					costToReach, optimalPath, crossPathIdx, kLen, foundCoord, foundDir, foundCost
				);

				for (const auto& step : optimalPath) {
					uniqueCoords.insert(step.first);
				}

				if (found) {
					uniqueCoords.insert(foundCoord);
				}
			}
		}

		void addPotentialLowestPath(
			std::priority_queue<state_t, std::vector<state_t>, CompareStates>& priorityQueue, int& iter, const size_t kLen,
			long& lowestCostToEnd, std::unordered_map<coord_t, long, PairHash>& costToReach, coord_t& endPosition, std::vector<coordn_dir_t>& optimalPath)
		{
			long currentCost = -1;
			while (!priorityQueue.empty() && iter < kMaxIter) {
				++iter;
				if (iter % 75'000 == 0) std::cout << "iter: " << iter << '\n';
				state_t currentState = priorityQueue.top();
				priorityQueue.pop();

				currentCost = std::get<0>(currentState);
				coord_t currentPosition = std::get<1>(currentState);
				Direction currentDirection = std::get<2>(currentState);
				std::vector<coordn_dir_t> currentPath = std::get<3>(currentState);

				/*if (toLog) {
					std::cout << "Iteration: " << iter << ", Current Cost: " << currentCost << '\n';
					coutPathAsGrid(currentPath, kLen);
				}*/

				if (currentCost > lowestCostToEnd) {
					continue; // invalid path
				}

				if (costToReach.find(currentPosition) != costToReach.end() &&
					!isMultiSplitCrossRoads(currentPosition, kLen) &&
					currentCost >= costToReach[currentPosition]) {
					continue;
				}

				if (costToReach[currentPosition] == checkedPathCoordCost) {
					continue;
				}
				else {
					costToReach[currentPosition] = currentCost;
				}

				//				if (toLog) coutGridWithPos(currentPosition, kLen, "\n");

				if (currentPosition == endPosition) {
					if (currentCost < lowestCostToEnd) {
						lowestCostToEnd = currentCost;
						optimalPath = currentPath;
					}
					continue;
				}

				for (int directionIndex = 0; directionIndex < 3; ++directionIndex) {
					Direction newDirection = currentDirection;
					if (directionIndex == 1) {
						newDirection = turnLeft(currentDirection);
					}
					else if (directionIndex == 2) {
						newDirection = turnRight(currentDirection);
					}

					coord_t newPosition = moveForward(currentPosition, newDirection);

					if (isValidMove(newPosition, kLen)) {
						long newCost = currentCost + 1;
						if (newDirection != currentDirection) {
							newCost += turnCost;
						}

						if (costToReach.find(newPosition) != costToReach.end() &&
							!isMultiSplitCrossRoads(newPosition, kLen) &&
							newCost >= costToReach[newPosition]) {
							continue;
						}

						std::vector<coordn_dir_t> newPath = currentPath;
						newPath.push_back(std::make_pair(newPosition, newDirection));

						priorityQueue.push(std::make_tuple(newCost, newPosition, newDirection, newPath));
					}
				}
			}
			if (toLog) std::cout << "cost: " << currentCost << '\n';
		}

		size_t getValidPathDijkstra(const size_t kLen) {
			std::pair<coord_t, coord_t> startAndEndPositions = findStartAndEndPositions(kLen);
			coord_t startPosition = startAndEndPositions.first;
			coord_t endPosition = startAndEndPositions.second;

			std::vector<coordn_dir_t> optimalPath;
			std::unordered_map<coord_t, long, PairHash> costToReach;
			costToReach[startPosition] = 1;
			long lowestCostToEnd = std::numeric_limits<long>::max();
			const std::vector<coordn_dir_t> initialPath = { std::make_pair(startPosition, startDirection) };
			std::vector<std::pair<coord_t, Direction>> crossRoadsCoords;

			int iter = 0;
			std::priority_queue<state_t, std::vector<state_t>, CompareStates> priorityQueue;
			priorityQueue.push(std::make_tuple(0, startPosition, startDirection, initialPath));

			std::unordered_set<coord_t, PairHash> uniqueCoords;

			addPotentialLowestPath(priorityQueue, iter, kLen, lowestCostToEnd, costToReach, endPosition, optimalPath);

			for (const auto& step : optimalPath) {
				if (isMultiSplitCrossRoads(step.first, kLen)) {
					crossRoadsCoords.push_back(std::make_pair(step.first, step.second));
				}
			}

			crossroadsCalc(optimalPath, crossRoadsCoords, costToReach, kLen, uniqueCoords);

			if (toLog) {
				coutPathIndexGrid(optimalPath, kLen);
				coutCostToReachGrid(costToReach, kLen);
				coutUniqueCoords(uniqueCoords);
			}
			return uniqueCoords.size();
		}

		size_t doMath(const size_t kLen)
		{
			size_t test = getValidPathDijkstra(kLen);
			return test;
		}

		void handleFile(std::ifstream& inputFile)
		{
			if (inputFile.is_open()) {
				std::string line;
				int currentRow = 0;
				size_t kLen = 0;
				bool firstLine = true;
				while (getline(inputFile, line)) {
					if (firstLine) {
						kLen = line.length();
						initializeGrid(kLen);
						firstLine = false;
					}
					fillLineInVector(line, kLen, currentRow);
					currentRow++;
				}

				std::cout << "Finished reading file\n";

				inputFile.close(); // automatically happens when going out of scope but no longer needed. More about explicitness.
				if (kLen == 0) throw std::runtime_error("Failed to define kLen");
				size_t total = doMath(kLen);
				std::cout << "Lowest: " << total << '\n';

				std::cout << "\nFinished running program";
			}
			else {
				std::cout << "Unable to open file\n";
			}
		}
		Direction turnLeft(Direction dir) {
			return static_cast<Direction>((dir + 3) % 4);
		}

		Direction turnRight(Direction dir) {
			return static_cast<Direction>((dir + 1) % 4);
		}
		bool isValidMove(const coord_t& pos, const size_t kLen) {
			if (pos.first < 0 || pos.first >= kLen || pos.second < 0 || pos.second >= kLen) {
				return false;
			}
			if (grid[pos.first][pos.second] == GridChar::Wall) {
				return false;
			}
			return true;
		}

		coord_t moveForward(const coord_t& pos, Direction dir) {
			switch (dir) {
			case Direction::North: return std::make_pair(pos.first - 1, pos.second);
			case Direction::East: return std::make_pair(pos.first, pos.second + 1);
			case Direction::South: return std::make_pair(pos.first + 1, pos.second);
			case Direction::West: return std::make_pair(pos.first, pos.second - 1);
			}
			throw std::runtime_error("Invalid direction");
		}
		std::pair<coord_t, coord_t> findStartAndEndPositions(const size_t kLen) {
			coord_t startPos;
			coord_t endPos;

			for (size_t row = 0; row < kLen; ++row) {
				for (size_t col = 0; col < kLen; ++col) {
					if (grid[row][col] == GridChar::Start) {
						startPos = { row, col };
					}
					if (grid[row][col] == GridChar::End) {
						endPos = { row, col };
					}
				}
			}

			return std::make_pair(startPos, endPos);
		}
#pragma region cout
		void coutPathIndexGrid(const std::vector<coordn_dir_t>& path, const size_t kLen) {
			std::cout << '\n';
			std::unordered_map<coord_t, size_t, PairHash> coordToIndex;
			for (size_t i = 0; i < path.size(); ++i) {
				coordToIndex[path[i].first] = i;
			}

			for (size_t row = 0; row < kLen; ++row) {
				for (size_t col = 0; col < kLen; ++col) {
					coord_t coord = { row, col };
					if (grid[row][col] == GridChar::Wall) {
						std::cout << "##";
					}
					else if (grid[row][col] == GridChar::Empty || grid[row][col] == GridChar::Start || grid[row][col] == GridChar::End) {
						if (coordToIndex.find(coord) != coordToIndex.end()) {
							size_t index = coordToIndex.at(coord);
							if (index > 99999) {
#ifdef _WIN32
								HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
								SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
								std::cout << "ER";
								SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
								std::cout << "ER";
#endif
							}
							else {
#ifdef _WIN32
								HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
								SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
								size_t secondThirdDigits = (index / 10) % 100;
								std::cout << std::setw(2) << std::setfill('0') << secondThirdDigits;
								SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
								size_t secondThirdDigits = (index / 10) % 100;
								std::cout << std::setw(2) << std::setfill('0') << secondThirdDigits;
#endif
							}
						}
						else {
							std::cout << "..";
						}
					}
					else {
						std::cout << "??";
					}
				}
				std::cout << '\n';
			}
			std::cout << '\n';
		}
		void coutCostToReachGrid(const std::unordered_map<coord_t, long, PairHash>& costToReach, const size_t kLen) {
			for (size_t row = 0; row < kLen; ++row) {
				for (size_t col = 0; col < kLen; ++col) {
					coord_t coord = { row, col };
					if (grid[row][col] == GridChar::Wall) {
						std::cout << "##";
					}
					else if (grid[row][col] == GridChar::Empty) {
						if (costToReach.find(coord) != costToReach.end()) {
							long cost = costToReach.at(coord);
							if (cost > 999'999) {
#ifdef _WIN32
								HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
								SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
								std::cout << "ER";
								SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
								std::cout << "ER";
#endif
							}
							else {
#ifdef _WIN32
								HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

								// Determine 10K increment range and cycle through 6 colors
								int rangeIndex = static_cast<int>(cost / 10000) % 6;

								// Set repeating rainbow color pattern: Red-Yellow-Green-Cyan-Blue-Magenta
								WORD colorAttribute;
								switch (rangeIndex) {
								case 0: colorAttribute = FOREGROUND_RED | FOREGROUND_INTENSITY; break; // Red
								case 1: colorAttribute = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; break; // Yellow
								case 2: colorAttribute = FOREGROUND_GREEN | FOREGROUND_INTENSITY; break; // Green
								case 3: colorAttribute = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY; break; // Cyan
								case 4: colorAttribute = FOREGROUND_BLUE | FOREGROUND_INTENSITY; break; // Blue
								case 5: colorAttribute = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY; break; // Magenta
								default: colorAttribute = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; break; // Default White
								}

								SetConsoleTextAttribute(hConsole, colorAttribute);
								long fourthFifthDigits = (cost / 1000) % 100;
								std::cout << std::setw(2) << std::setfill('0') << fourthFifthDigits;
								SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
								long lastTwoDigits = cost % 100;
								std::cout << std::setw(2) << std::setfill('0') << lastTwoDigits;
#endif
							}
						}
						else {
							std::cout << "..";
						}
					}
					else {
						std::cout << "??";
					}
				}
				std::cout << '\n';
			}
			std::cout << '\n';
		}
		void coutGridWithPos(const coord_t& pos, const size_t kLen, const std::string prefix) {
			std::cout << prefix;
			for (size_t row = 0; row < kLen; ++row) {
				for (size_t col = 0; col < kLen; ++col) {
					if (row == pos.first && col == pos.second) {
#ifdef _WIN32
						HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
						SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
						std::cout << "XX";
						SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
						std::cout << 'XX';
#endif
					}
					else {
						std::cout << grid[row][col] << grid[row][col];
					}
				}
				std::cout << '\n';
			}
			std::cout << '\n';
		}
		void coutPathAsGrid(const std::vector<coordn_dir_t>& path, const size_t kLen) {
			char_square_t gridCopy = grid;

			for (const auto& step : path) {
				coord_t pos = step.first;
				char stepChar = '.';
				switch (step.second) {
				case Direction::North: stepChar = '^'; break;
				case Direction::East: stepChar = '>'; break;
				case Direction::South: stepChar = 'v'; break;
				case Direction::West: stepChar = '<'; break;
				}
				gridCopy[pos.first][pos.second] = stepChar;
			}

			for (size_t row = 0; row < kLen; ++row) {
				for (size_t col = 0; col < kLen; ++col) {
					std::cout << gridCopy[row][col];
				}
				std::cout << '\n';
			}
			std::cout << '\n';
		}
#pragma endregion
#pragma region startState
		void initializeGrid(const size_t kLen) {
			grid.resize(kLen, std::vector<char>(kLen, GridChar::Empty));
		}
		void fillLineInVector(const std::string& line, const size_t kLen, int currentRow) {
			for (size_t col = 0; col < kLen; ++col) {
				grid[currentRow][col] = line[col];
			}
		}
#pragma endregion
	}
	void daySixteenPartTwo() {
		std::system("cls"); // clear terminal pre-boot
		std::cout << "Running program DaySixteenPartTwo" << '\n';
		const bool isFullFile = false; // true   false

		std::string line;
		std::ifstream inputFile;
		if (isFullFile) {
			std::cout << "Using full file\n\n";
		}
		else {
			std::cout << "Using test file\n\n";
		}
		(isFullFile) ? inputFile.open("daySixteenFull.txt") : inputFile.open("daySixteenTest.txt");
		handleFile(inputFile);
	}
}
// 105512 too high
// off by 4 
