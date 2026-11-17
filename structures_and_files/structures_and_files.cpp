/*******************************
 * Автор:    Рычков Михаил     *
 * Название: Структуры и файлы *
 * Вариант:  24                *
 *******************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

struct Participant {
  string surname;
  string firstName;
  string middleName;
  string degree;
  string topic;
  double duration;
};

vector<string> splitFields(const string& line) {
  vector<string> fields;
  size_t start = 0;

  while (true) {
    size_t separator = line.find(';', start);

    if (separator == string::npos) {
      fields.push_back(line.substr(start));
      break;
    }

    fields.push_back(line.substr(start, separator - start));
    start = separator + 1;
  }

  return fields;
}

bool isValidText(const string& text) {
  if (text.empty()) {
    return false;
  }

  for (size_t symbolIndex = 0; symbolIndex < text.size(); ++symbolIndex) {
    if (static_cast<unsigned char>(text[symbolIndex]) > 127) {
      return false;
    }
  }

  return true;
}

bool readDouble(const string& text, double& value) {
  istringstream parser(text);
  char extra;

  if (!(parser >> value) || !isfinite(value)) {
    return false;
  }

  return !(parser >> extra);
}

bool readRecords(const string& inputPath, vector<Participant>& records, string& errorMessage) {
  ifstream input(inputPath);

  if (!input) {
    errorMessage = "ERROR open";
    return false;
  }

  string line;
  int lineNumber = 0;
  constexpr int maxRecordCount = 100;

  while (getline(input, line)) {
    ++lineNumber;

    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }

    vector<string> fields = splitFields(line);

    if (fields.size() != 6 || !isValidText(fields[0]) || !isValidText(fields[1]) || !isValidText(fields[2]) || !isValidText(fields[3]) || !isValidText(fields[4])) {
      errorMessage = "ERROR line " + to_string(lineNumber);
      return false;
    }

    Participant current;

    current.surname = fields[0];
    current.firstName = fields[1];
    current.middleName = fields[2];
    current.degree = fields[3];
    current.topic = fields[4];

    if (!readDouble(fields[5], current.duration) || current.duration < 0) {
      errorMessage = "ERROR line " + to_string(lineNumber);
      return false;
    }

    if (records.size() >= maxRecordCount) {
      errorMessage = "ERROR line " + to_string(lineNumber);
      return false;
    }

    records.push_back(current);
  }

  if (input.bad()) {
    errorMessage = "ERROR read";
    return false;
  }

  return true;
}

string buildReport(const vector<Participant>& records, const string& requestedDegree) {
  ostringstream report;
  double totalDuration = 0;
  int selectedCount = 0;

  report << "Participants with degree: " << requestedDegree << '\n';

  for (size_t recordIndex = 0; recordIndex < records.size(); ++recordIndex) {
    const Participant& current = records[recordIndex];

    if (current.degree == requestedDegree) {
      report << current.surname << " "
             << current.firstName << " "
             << current.middleName << " | "
             << current.degree << " | "
             << current.topic << " | "
             << current.duration << " minutes" << '\n';

      totalDuration += current.duration;
      ++selectedCount;
    }
  }

  if (selectedCount == 0) {
    report << "NONE" << '\n';
    report << "Average duration: NONE" << '\n';
  } else {
    report << "Average duration: "
           << totalDuration / selectedCount << " minutes" << '\n';
  }

  report << "Shortest presentation among all participants:" << '\n';

  if (records.empty()) {
    report << "NONE" << '\n';
  } else {
    size_t shortestIndex = 0;

    for (size_t recordIndex = 1; recordIndex < records.size(); ++recordIndex) {
      if (records[recordIndex].duration < records[shortestIndex].duration) {
        shortestIndex = recordIndex;
      }
    }

    const Participant& shortest = records[shortestIndex];

    report << shortest.surname << " "
           << shortest.firstName << " "
           << shortest.middleName << " | "
           << shortest.degree << " | "
           << shortest.topic << " | "
           << shortest.duration << " minutes" << '\n';
  }

  return report.str();
}

bool saveReport(const string& outputPath, const string& reportText) {
  ofstream output(outputPath);

  if (!output) {
    return false;
  }

  output << reportText;
  output.flush();

  if (!output) {
    return false;
  }

  return true;
}

int main() {
  string inputPath;
  string outputPath;
  string requestedDegree;
  string errorMessage;
  vector<Participant> records;

  cout << "Enter input file path: ";
  getline(cin, inputPath);

  cout << "Enter output file path: ";
  getline(cin, outputPath);

  cout << "Enter the academic degree: ";
  getline(cin, requestedDegree);

  if (!isValidText(inputPath) || !isValidText(outputPath) || !isValidText(requestedDegree)) {
    cout << "INVALID" << endl;
    return 1;
  }

  if (!readRecords(inputPath, records, errorMessage)) {
    cout << errorMessage << endl;
    return 1;
  }

  string reportText = buildReport(records, requestedDegree);

  if (!saveReport(outputPath, reportText)) {
    cout << "ERROR output" << endl;
    return 1;
  }

  cout << reportText;

  return 0;
}