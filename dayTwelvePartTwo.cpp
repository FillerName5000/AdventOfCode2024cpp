#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set> // no default pair hashing in c++14
#include <map>

namespace DayTwelvePartTwo // The most difficult one so far.
{
	namespace {
		using square_vect_t = std::vector<std::vector<char>>;
		using coord_t = std::pair<int, int>;

		enum class Direction { Up, Right, Down, Left, None, Invalid };

		bool isCoordWithinBounds(const coord_t& nextCoord, const int kSquareLen);
		bool hasCharAtCoord(const square_vect_t& charSquare, const char prevChar, const coord_t& coordToCheck, const int kSquareLen);
		void fillLineInVector(const std::string line, square_vect_t& charSquare, const int kSquareLen, const int currentRow);

		void coutCharSquareWithChecked(const square_vect_t& charSquare, const int kSquareLen);
		void coutRegisteredBordersOverlay(const std::set<coord_t>& registeredBorders, const int kSquareLen, const std::set<coord_t>& priorityCoords, const std::set<coord_t>& otherCoords);
		void coutSetContents(const std::string prefix, const std::set<coord_t>& vec);
		void coutCorners(const int kSquareLen, const std::set<coord_t>& withinBorderCoords, std::set<coord_t>& cornersAdded);
		void coutSetAsGrid(const std::set<coord_t>& set, const int kSquareLen);

		Direction getOppositeDirection(Direction direction);
		void handleFile(std::ifstream& inputFile, square_vect_t& charSquare, const int kSquareLen);

		const bool toLog = true; // true   false
		const int kTestFileLineLen = 5;
		const int kFullFileLineLen = 141; // files validated and pre checked for length and valid chars only. 
		const std::vector<coord_t> kMovements = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };
		const size_t kMovementsSize = kMovements.size();

		std::set<coord_t> checkedCoords;
		std::map<char, std::set<coord_t>> charCoords;

		bool hasDiagonalInSet(const coord_t& nextCoord, const int kSquareLen, const std::set<coord_t>& withinBorderCoords) {
			for (int dx = -1; dx <= 1; ++dx) {
				for (int dy = -1; dy <= 1; ++dy) {
					if (std::abs(dx) != 1 || std::abs(dy) != 1) continue;
					coord_t diag = { nextCoord.first + dx, nextCoord.second + dy };
					if (isCoordWithinBounds(diag, kSquareLen) && withinBorderCoords.find(diag) != withinBorderCoords.end()) {
						return true;
					}
				}
			}
			return false;
		}

		bool isNinetyDegreeOuterBorder(const coord_t& borderCoord, const std::set<coord_t>& registeredBorders) {
			coord_t right = { borderCoord.first, borderCoord.second + 1 };
			coord_t down = { borderCoord.first + 1, borderCoord.second };
			coord_t left = { borderCoord.first, borderCoord.second - 1 };
			coord_t up = { borderCoord.first - 1, borderCoord.second };

			if (registeredBorders.find(right) != registeredBorders.end() && registeredBorders.find(down) != registeredBorders.end()) return true;
			if (registeredBorders.find(right) != registeredBorders.end() && registeredBorders.find(up) != registeredBorders.end()) return true;
			if (registeredBorders.find(left) != registeredBorders.end() && registeredBorders.find(down) != registeredBorders.end()) return true;
			if (registeredBorders.find(left) != registeredBorders.end() && registeredBorders.find(up) != registeredBorders.end()) return true;
			return false;
		}

		bool isUTurn(const std::set<coord_t>& withinBorderCoords, const coord_t& borderCoord) {
			int count = 0;
			for (const auto& move : kMovements) {
				coord_t ortho = { borderCoord.first + move.first, borderCoord.second + move.second };
				if (withinBorderCoords.find(ortho) != withinBorderCoords.end()) ++count;
			}
			return count >= 3;
		}

		bool isNinetyDegreeInnerBorder(const std::set<coord_t>& withinBorderCoords, const coord_t& borderCoord) {
			int count = 0;
			for (const auto& move : kMovements) {
				coord_t ortho = { borderCoord.first + move.first, borderCoord.second + move.second };
				if (withinBorderCoords.find(ortho) != withinBorderCoords.end()) ++count;
			}
			return count >= 2;
		}


		size_t getDiagResultUTurn(const coord_t& border, const std::set<coord_t>& withinBorderCoords)
		{
			int diagCount = 0;
			for (int dx = -1; dx <= 1; ++dx) {
				for (int dy = -1; dy <= 1; ++dy) {
					if (std::abs(dx) != 1 || std::abs(dy) != 1) continue;
					coord_t diag = { border.first + dx, border.second + dy };
					if (withinBorderCoords.find(diag) != withinBorderCoords.end()) ++diagCount;
				}
			}
			if (diagCount == 1) return 3;
			else if (diagCount == 2) return 3;
			else if (diagCount == 3) return 2;
			else if (diagCount == 4) return 1;
			else {
				return 4; // locked in
			}
		}

		size_t getTotalCornersFromFullSet(const int kSquareLen, const std::set<coord_t> withinBorderCoords, std::set<coord_t> allBorders) {
			size_t totalCorners = 0;
			std::set<coord_t> cornersAdded;

			if (withinBorderCoords.size() == 1) {
				std::cout << "Single coord (" << (*withinBorderCoords.begin()).first << "," << (*withinBorderCoords.begin()).second << ") adds 4 corners\n";
				return 4;
			}

			for (const auto& border : allBorders) {
				size_t cornerCount = 0;
				if (isUTurn(withinBorderCoords, border)) {
					cornerCount = getDiagResultUTurn(border, withinBorderCoords);
					totalCorners += cornerCount;
					cornersAdded.insert(border);
					std::cout << "UTurn at (" << border.first << "," << border.second << ") adds " << cornerCount << " corners\n";
				}
				else if (isNinetyDegreeInnerBorder(withinBorderCoords, border)) {
					cornerCount = 1;
					totalCorners += cornerCount;
					cornersAdded.insert(border);
					std::cout << "Inner90 at (" << border.first << "," << border.second << ") adds " << cornerCount << " corner\n";
				}
				else if (isNinetyDegreeOuterBorder(border, allBorders)) {
					cornerCount = 1;
					totalCorners += cornerCount;
					cornersAdded.insert(border);
					std::cout << "Outer90 at (" << border.first << "," << border.second << ") adds " << cornerCount << " corner\n";
				}
			}

			if (toLog) coutCorners(kSquareLen, withinBorderCoords, cornersAdded);

			return totalCorners;
		}

		size_t getTotalBorders(const square_vect_t& charSquare, const int kSquareLen, const char charInBorder, const std::set<coord_t>& registeredBorders, std::set<coord_t> withinBorderCoords) {
			std::set<coord_t> checkedCorners = registeredBorders;
			std::set<coord_t> checkedCornersWithImpact;

			for (const auto& border : registeredBorders) { // adds corners not orthogonally connected to an inside char
				for (size_t i = 0; i < kMovementsSize; ++i) {
					coord_t nextCoord = { border.first + kMovements[i].first, border.second + kMovements[i].second };
					if (checkedCorners.find(nextCoord) != checkedCorners.end()) continue;
					if (hasCharAtCoord(charSquare, charInBorder, nextCoord, kSquareLen)) {
					//	if (toLog) std::cout << "nextCoord: (x:" << nextCoord.second << ",y:" << nextCoord.first << ") for " << charInBorder << "\n";

						if (withinBorderCoords.find(nextCoord) == withinBorderCoords.end()) {
							checkedCorners.insert(nextCoord);
							checkedCornersWithImpact.insert(nextCoord);
						}

						continue;
					}

					int orthogonalCount = 0;
					for (size_t j = 0; j < kMovementsSize; ++j) {
						if (!hasDiagonalInSet(nextCoord, kSquareLen, withinBorderCoords)) continue;
						coord_t orthoCoord = { nextCoord.first + kMovements[j].first, nextCoord.second + kMovements[j].second };
						if (registeredBorders.find(orthoCoord) != registeredBorders.end()) {
							++orthogonalCount;
						}
					}

					if (orthogonalCount >= 2) {
						checkedCornersWithImpact.insert(nextCoord);
						checkedCorners.insert(nextCoord);
					}
				}
			}

			if (toLog) coutRegisteredBordersOverlay(registeredBorders, kSquareLen, checkedCornersWithImpact, withinBorderCoords);

			std::set<coord_t> allBorders;
			allBorders.insert(registeredBorders.begin(), registeredBorders.end());
			allBorders.insert(checkedCorners.begin(), checkedCorners.end());
			size_t totalCorners = getTotalCornersFromFullSet(kSquareLen, withinBorderCoords, allBorders);

			return totalCorners;
		}

		size_t getAreaPrice(const square_vect_t& charSquare, const int kSquareLen, const coord_t& startCoord) {
			size_t entriesChecked = 0;

			std::set<coord_t> registeredBorders;

			std::vector<std::pair<coord_t, Direction>> coordsWithOrigin;
			std::set<coord_t> allCoordsAdded;

			coordsWithOrigin.push_back(std::make_pair(startCoord, Direction::None));
			allCoordsAdded.insert(startCoord);

			while (!coordsWithOrigin.empty()) {
				std::pair<coord_t, Direction> top = coordsWithOrigin.back();
				coord_t currentCoord = top.first;
				Direction prevDirection = top.second;
				coordsWithOrigin.pop_back();

				if (checkedCoords.find(currentCoord) != checkedCoords.end()) continue;
				checkedCoords.insert(currentCoord);
				++entriesChecked;

				for (int i = 0; i < kMovementsSize; ++i) {
					Direction currentDirection = static_cast<Direction>(i);
					if (prevDirection == getOppositeDirection(prevDirection)) continue;

					coord_t nextCoord = { currentCoord.first + kMovements[i].first, currentCoord.second + kMovements[i].second };

					if (hasCharAtCoord(charSquare, charSquare[currentCoord.first][currentCoord.second], nextCoord, kSquareLen)) {
						coordsWithOrigin.push_back(std::make_pair(nextCoord, currentDirection));
						allCoordsAdded.insert(nextCoord);
					}
					else {
						registeredBorders.insert(nextCoord);
					}
				}
			}

			size_t totalBorders = getTotalBorders(charSquare, kSquareLen, charSquare[startCoord.first][startCoord.second], registeredBorders, allCoordsAdded);

			//if (toLog) 
			std::cout << "Total for: " << charSquare[startCoord.first][startCoord.second] << " is " << entriesChecked << " * " << totalBorders << '\n';

			return totalBorders * entriesChecked;
		}

		size_t calculateTotalPrice(const square_vect_t& charSquare, const int kSquareLen) {
			size_t total = 0;

			for (int row = 0; row < kSquareLen; ++row) {
				for (int col = 0; col < kSquareLen; ++col) {
					coord_t currentCoord = { row, col };
					if (checkedCoords.find(currentCoord) == checkedCoords.end()) {
						total += getAreaPrice(charSquare, kSquareLen, currentCoord);
					}
				}
			}

			return total;
		}

		void handleFile(std::ifstream& inputFile, square_vect_t& charSquare, const int kSquareLen)
		{
			std::pair<int, int> startingGuardCoord;
			if (inputFile.is_open()) {
				std::string line;
				int currentRow = 0;
				while (getline(inputFile, line)) {
					if (currentRow >= kSquareLen) throw std::runtime_error("Line read out of bounds");
					fillLineInVector(line, charSquare, kSquareLen, currentRow);
					currentRow++;
				}

				std::cout << "Finished reading file\n";

				inputFile.close(); // automatically happens when going out of scope but no longer needed. More about explicitness.

				size_t totalPrice = calculateTotalPrice(charSquare, kSquareLen);

				std::cout << "Total price: " << totalPrice;

				std::cout << "\nFinished running program";
			}
			else {
				std::cout << "Unable to open file\n";
			}
		}


		bool hasCharAtCoord(const square_vect_t& charSquare, const char prevChar, const coord_t& coordToCheck, const int kSquareLen) {
			if (!isCoordWithinBounds(coordToCheck, kSquareLen)) return false;
			return (charSquare[coordToCheck.first][coordToCheck.second] == prevChar);
		}

		bool isCoordWithinBounds(const coord_t& nextCoord, const int kSquareLen)
		{
			return nextCoord.first >= 0 && nextCoord.first < kSquareLen && nextCoord.second >= 0 && nextCoord.second < kSquareLen;
		}

		void fillLineInVector(const std::string line, square_vect_t& charSquare, const int kSquareLen, const int currentRow) {
			for (int col = 0; col < kSquareLen; ++col) {
				charSquare[currentRow][col] = line[col];
			}
		}

		Direction getOppositeDirection(Direction direction) {
			switch (direction) {
			case Direction::Up: return Direction::Down;
			case Direction::Right: return Direction::Left;
			case Direction::Down: return Direction::Up;
			case Direction::Left: return Direction::Right;
			default: return Direction::Invalid;
			}
		}

		//-----------------LOGGING---------------------------------------------------------------

		void coutSetContents(const std::string prefix, const std::set<coord_t>& vec) {
			std::cout << prefix;
			for (const auto& c : vec) {
				std::cout << "(" << c.first << "," << c.second << ") ";
			}
			std::cout << '\n';
		}

		void coutCorners(const int kSquareLen, const std::set<coord_t>& withinBorderCoords, std::set<coord_t>& cornersAdded) {
			int minX = 0, minY = 0, maxX = kSquareLen - 1, maxY = kSquareLen - 1;
			for (const auto& c : withinBorderCoords) {
				if (c.first < minX) minX = c.first;
				if (c.second < minY) minY = c.second;
				if (c.first > maxX) maxX = c.first;
				if (c.second > maxY) maxY = c.second;
			}
			for (const auto& c : cornersAdded) {
				if (c.first < minX) minX = c.first;
				if (c.second < minY) minY = c.second;
				if (c.first > maxX) maxX = c.first;
				if (c.second > maxY) maxY = c.second;
			}
			int gridRows = maxX - minX + 3;
			int gridCols = maxY - minY + 3;
			std::vector<std::vector<char>> grid(gridRows, std::vector<char>(gridCols, '.'));
			for (const auto& c : withinBorderCoords) {
				int x = c.first - minX + 1, y = c.second - minY + 1;
				if (x >= 0 && x < gridRows && y >= 0 && y < gridCols) grid[x][y] = 'c';
			}
			for (const auto& c : cornersAdded) {
				int x = c.first - minX + 1, y = c.second - minY + 1;
				if (x >= 0 && x < gridRows && y >= 0 && y < gridCols) grid[x][y] = 'X';
			}
			std::cout << "Corners:\n";
			for (const auto& row : grid) {
				for (const auto& cell : row) std::cout << cell;
				std::cout << '\n';
			}
		}

		void coutSetAsGrid(const std::set<coord_t>& set, const int kSquareLen) {
			int gridLen = kSquareLen + 2;
			std::vector<std::vector<char>> grid(gridLen, std::vector<char>(gridLen, '.'));
			for (const auto& coord : set) {
				int x = coord.first + 1, y = coord.second + 1;
				if (x >= 0 && x < gridLen && y >= 0 && y < gridLen) grid[x][y] = '#';
			}
			for (const auto& row : grid) {
				for (const auto& cell : row) std::cout << cell;
				std::cout << '\n';
			}
		}


		void coutRegisteredBordersOverlay(const std::set<coord_t>& registeredBorders, const int kSquareLen, const std::set<coord_t>& priorityCoords, const std::set<coord_t>& otherCoords) {
			int gridLen = kSquareLen + 2;
			std::vector<std::vector<char>> grid(gridLen, std::vector<char>(gridLen, '.'));
			for (const auto& coord : otherCoords) {
				int x = coord.first + 1, y = coord.second + 1;
				if (x >= 0 && x < gridLen && y >= 0 && y < gridLen) grid[x][y] = 'c';
			}
			for (const auto& coord : registeredBorders) {
				int x = coord.first + 1, y = coord.second + 1;
				if (x >= 0 && x < gridLen && y >= 0 && y < gridLen) grid[x][y] = '#';
			}
			for (const auto& coord : priorityCoords) {
				int x = coord.first + 1, y = coord.second + 1;
				if (x >= 0 && x < gridLen && y >= 0 && y < gridLen) {
					if (grid[x][y] == '#') grid[x][y] = '&';
					else grid[x][y] = '+';
				}
			}
			for (const auto& row : grid) {
				for (const auto& cell : row) std::cout << cell;
				std::cout << '\n';
			}
		}

		void coutCharSquareWithChecked(const square_vect_t& charSquare, const int kSquareLen) {
			for (int row = 0; row < kSquareLen; ++row) {
				for (int col = 0; col < kSquareLen; ++col) {
					coord_t currentCoord = { row, col };
					if (checkedCoords.find(currentCoord) != checkedCoords.end()) {
						std::cout << '.';
					}
					else {
						std::cout << charSquare[row][col];
					}
				}
				std::cout << '\n';
			}
			std::cout << '\n';
		}

		//-----------------LOGGING---------------------------------------------------------------
	}

	void dayTwelvePartTwo() {
		std::system("cls"); // clear terminal pre-boot
		std::cout << "Running program DayTwelvePartTwo" << '\n';
		const bool isFullFile = false; // true   false
		const int kSquareLen = isFullFile ? kFullFileLineLen : kTestFileLineLen;
		square_vect_t charSquare(kSquareLen, std::vector<char>(kSquareLen));

		std::string line;
		std::ifstream inputFile;
		if (isFullFile) {
			std::cout << "Using full file\n\n";
		}
		else {
			std::cout << "Using test file\n\n";
		}
		(isFullFile) ? inputFile.open("dayTwelveFull.txt") : inputFile.open("dayTwelveTest.txt");
		handleFile(inputFile, charSquare, kSquareLen);
	}
}

// 928307 too low
// 930118 too high
// 930107 not right
// 931793 not right
// 937032 from test, unsubmitted