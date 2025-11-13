#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <limits>
#include <tuple>
#include <queue>

namespace DaySixteen
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
		void initializeGrid(const int kLen);
		void fillLineInVector(const std::string& line, const int kLen, int currentRow);
#pragma endregion
		using sequences_t = std::unordered_set<std::vector<coordn_dir_t>, VectorHash>;
		using state_t = std::tuple<long, coord_t, Direction, std::vector<coordn_dir_t>>;
		struct CompareStates {
			bool operator()(const state_t& stateA, const state_t& stateB) const {
				return std::get<0>(stateA) > std::get<0>(stateB); // B state must be smaller as true invokes a move up for the new element
				// from docs: using std::greater<T> would cause the smallest element to appear as the top(). 
			}
		};
		void coutGridWithPos(const coord_t& pos, const int kLen, const std::string prefix);
		void coutPathAsGrid(const std::vector<coordn_dir_t>& path, const int kLen);
		bool isValidMove(const coord_t& pos, const int kLen);
		coord_t moveForward(const coord_t& pos, Direction dir);
		Direction turnRight(Direction dir);
		Direction turnLeft(Direction dir);
		std::pair<coord_t, coord_t> findStartAndEndPositions(const int kLen);

		const bool toLog = false; // true   false
		const int kLenFull = 141;
		const int kLenTest = 10; // todo change this to use line len
		const int kMaxIter = 10'000'0; // Not enough for the iterative approach, time to use an actual path navigation algo (dijkstra)

		const int turnCost = 1000;
		const Direction startDirection = Direction::East;

		char_square_t grid;

		bool isMultiSplitCrossRoads(const coord_t& pos, const int kLen) {
			int validMoveCount = 0;
			for (int directionIndex = 0; directionIndex < 4; ++directionIndex) {
				Direction testDirection = static_cast<Direction>(directionIndex);
				coord_t testPosition = moveForward(pos, testDirection);
				if (isValidMove(testPosition, kLen)) {
					++validMoveCount;
				}
			}
			return validMoveCount >= 3;
		}

		long getValidPathDijkstra(const int kLen) {
			std::pair<coord_t, coord_t> startAndEndPositions = findStartAndEndPositions(kLen);
			coord_t startPosition = startAndEndPositions.first;
			coord_t endPosition = startAndEndPositions.second;

			std::priority_queue<state_t, std::vector<state_t>, CompareStates> priorityQueue;
			std::vector<coordn_dir_t> initialPath = { std::make_pair(startPosition, startDirection) };
			priorityQueue.push(std::make_tuple(0, startPosition, startDirection, initialPath));

			std::unordered_map<coord_t, long, PairHash> costToReach;
			costToReach[startPosition] = 1; // init offset
			long lowestCostToEnd = std::numeric_limits<long>::max();

			int iter = 0;
			while (!priorityQueue.empty() && iter < kMaxIter) {
				++iter;
				state_t currentState = priorityQueue.top();
				priorityQueue.pop();

				long currentCost = std::get<0>(currentState);
				coord_t currentPosition = std::get<1>(currentState);
				Direction currentDirection = std::get<2>(currentState);
				std::vector<coordn_dir_t> currentPath = std::get<3>(currentState);


				if (toLog) {
					std::cout << "Iteration: " << iter << ", Current Cost: " << currentCost << '\n';
					coutPathAsGrid(currentPath, kLen);
				}

				if (currentCost >= lowestCostToEnd) {
					continue;
				}

				if (costToReach.find(currentPosition) != costToReach.end()&& !isMultiSplitCrossRoads(currentPosition, kLen) && currentCost >= costToReach[currentPosition]) {
					continue;
				}

				costToReach[currentPosition] = currentCost;

				if (currentPosition == endPosition) {
					if (currentCost < lowestCostToEnd) {
						lowestCostToEnd = currentCost;
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

						if (costToReach.find(newPosition) != costToReach.end() && !isMultiSplitCrossRoads(newPosition, kLen) && newCost >= costToReach[newPosition]) {
							continue;
						}

						std::vector<coordn_dir_t> newPath = currentPath;
						newPath.push_back(std::make_pair(newPosition, newDirection));

						priorityQueue.push(std::make_tuple(newCost, newPosition, newDirection, newPath));
					}
				}
			}
			return lowestCostToEnd == std::numeric_limits<long>::max() ? -1 : lowestCostToEnd;
		}

		long doMath(const int kLen)
		{
			long lowestCost = getValidPathDijkstra(kLen);
			if (lowestCost == -1) {
				std::cout << "No valid path found.\n";
				return -1;
			}

			std::cout << "Lowest Cost: " << lowestCost << '\n';
			return lowestCost;
		}

		void handleFile(std::ifstream& inputFile, const int kLen)
		{
			if (inputFile.is_open()) {
				initializeGrid(kLen);
				std::string line;
				int currentRow = 0;
				while (getline(inputFile, line)) {
					fillLineInVector(line, kLen, currentRow);
					currentRow++;
				}

				std::cout << "Finished reading file\n";

				inputFile.close(); // automatically happens when going out of scope but no longer needed. More about explicitness.

				long total = doMath(kLen);
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
		bool isValidMove(const coord_t& pos, const int kLen) {
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
		std::pair<coord_t, coord_t> findStartAndEndPositions(const int kLen) {
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
		void coutGridWithPos(const coord_t& pos, const int kLen, const std::string prefix) {
			std::cout << prefix;
			for (size_t row = 0; row < kLen; ++row) {
				for (size_t col = 0; col < kLen; ++col) {
					if (row == pos.first && col == pos.second) {
						std::cout << 'X';
					}
					else {
						std::cout << grid[row][col];
					}
				}
				std::cout << '\n';
			}
			std::cout << '\n';
		}
		void coutPathAsGrid(const std::vector<coordn_dir_t>& path, const int kLen) {
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
		void initializeGrid(const int kLen) {
			grid.resize(kLen, std::vector<char>(kLen, GridChar::Empty));
		}
		void fillLineInVector(const std::string& line, const int kLen, int currentRow) {
			for (int col = 0; col < kLen; ++col) {
				grid[currentRow][col] = line[col];
			}
		}
#pragma endregion
	}
	void daySixteen() {
		std::system("cls"); // clear terminal pre-boot
		std::cout << "Running program DaySixteen" << '\n';
		const bool isFullFile = true; // true   false
		const int kLen = isFullFile ? kLenFull : kLenTest;

		std::string line;
		std::ifstream inputFile;
		if (isFullFile) {
			std::cout << "Using full file\n\n";
		}
		else {
			std::cout << "Using test file\n\n";
		}
		(isFullFile) ? inputFile.open("daySixteenFull.txt") : inputFile.open("daySixteenTest.txt");
		handleFile(inputFile, kLen);
	}
}
// 105512 too high
// off by 4 