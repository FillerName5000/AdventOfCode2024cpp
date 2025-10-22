#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <set>

namespace DayEightPartTwo
{
	namespace {
		using square_vect_t = std::vector<std::vector<char>>;
		using coord_t = std::pair<int, int>;
		coord_t getReverseCoordMovement(const coord_t movementCopy);
		bool isCoordWithinBounds(const coord_t& nextCoord, const int kSquareLen);
		void coutSquareVector(const std::string prefix, const square_vect_t& charSquare);
		coord_t getAToBMovement(const coord_t a, const coord_t b);
		void coutModifiedSquareVector(square_vect_t charSquareCopy, const std::set<coord_t>& antinodes);

		const bool toLog = false; // true   false
		const int kTestFileLineLen = 12;
		const int kFullFileLineLen = 50; // files validated and pre checked for length and valid chars only. 

		const char kEmptySpaceChar = '.';

		std::unordered_map<char, std::vector<coord_t>> antennas;
		std::set<coord_t> antinodes;

		void insertAntinodeLineCoords(const coord_t& startCoord, const coord_t& movement, const size_t kSquareLen, std::set<coord_t>& antinodes) {
			coord_t currentCoord = startCoord;
			while (isCoordWithinBounds(currentCoord, kSquareLen)) {
				if (antinodes.find(currentCoord) == antinodes.end()) {
					antinodes.insert(currentCoord);
				}
				currentCoord.first += movement.first;
				currentCoord.second += movement.second;
			}
		}

		void insertAntiNodesCoords(std::vector<coord_t> antennaCharCoords, const size_t kSquareLen, const square_vect_t charSquare)
		{
			size_t size = antennaCharCoords.size();
			if (size < 2) return;

			for (size_t i = 0; i < size; ++i) {
				for (size_t j = i + 1; j < size; ++j) {
					coord_t movementToB = getAToBMovement(antennaCharCoords[i], antennaCharCoords[j]);
					coord_t reverseMovement = getReverseCoordMovement(movementToB);
					insertAntinodeLineCoords(antennaCharCoords[i], reverseMovement, kSquareLen, antinodes);
					insertAntinodeLineCoords(antennaCharCoords[j], movementToB, kSquareLen, antinodes);
				}
				if (toLog) coutModifiedSquareVector(charSquare, antinodes);
			}
		}

		size_t calculateAntinodes(const square_vect_t charSquare, const int kSquareLen)
		{
			for (const auto& antennaCharCoords : antennas) {
				insertAntiNodesCoords(antennaCharCoords.second, kSquareLen, charSquare);
			}

			if (toLog) coutModifiedSquareVector(charSquare, antinodes);

			return antinodes.size();
		}

		void fillLineInVector(const std::string line, square_vect_t& charSquare, const int kSquareLen, const int currentRow) {
			for (int col = 0; col < kSquareLen; ++col) {
				charSquare[currentRow][col] = line[col];
				if (line[col] != kEmptySpaceChar) antennas[line[col]].push_back({ currentRow, col });
			}
		}

		void handleFile(std::ifstream& inputFile, square_vect_t charSquare, const int kSquareLen)
		{
			coord_t startingGuardCoord;
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

				size_t antiNodes = calculateAntinodes(charSquare, kSquareLen);

				std::cout << "Antinodes: " << antiNodes << '\n';

				std::cout << "\nFinished running program";
			}
			else {
				std::cout << "Unable to open file\n";
			}
		}

		coord_t getReverseCoordMovement(coord_t movementCopy)
		{
			return { -movementCopy.first, -movementCopy.second };
		}

		coord_t getAToBMovement(const coord_t a, const coord_t b)
		{
			coord_t movement = { b.first - a.first, b.second - a.second };
			return movement;
		}

		bool isCoordWithinBounds(const coord_t& nextCoord, const int kSquareLen)
		{
			return nextCoord.first >= 0 && nextCoord.first < kSquareLen && nextCoord.second >= 0 && nextCoord.second < kSquareLen;
		}

		void coutModifiedSquareVector(square_vect_t charSquareCopy, const std::set<coord_t>& antinodes) {
			for (const auto& coord : antinodes) {
				int row = coord.first;
				int col = coord.second;
				if (row >= 0 && row < charSquareCopy.size() && col >= 0 && col < charSquareCopy[row].size()) {
					charSquareCopy[row][col] = (charSquareCopy[row][col] == kEmptySpaceChar) ? '!' : '&';
				}
			}

			std::cout << "Modified Character Square:\n";
			for (const auto& row : charSquareCopy) {
				for (const auto& ch : row) {
					std::cout << ch;
				}
				std::cout << '\n';
			}
			std::cout << '\n';
		}

		void coutSquareVector(const std::string prefix, const square_vect_t& charSquare) {
			std::cout << prefix << '\n';
			for (const auto& row : charSquare) {
				for (const auto& ch : row) {
					std::cout << ch;
				}
				std::cout << '\n';
			}
			std::cout << '\n';
		}
	}
	void dayEightPartTwo() { // Antenna coord can have an antinode
		std::system("cls"); // clear terminal pre-boot
		std::cout << "Running program Day EightPartTwo" << '\n';
		const bool isFullFile = true; // true   false
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
		(isFullFile) ? inputFile.open("dayEightFull.txt") : inputFile.open("dayEightTest.txt");
		handleFile(inputFile, charSquare, kSquareLen);
	}
}
// 434 too high