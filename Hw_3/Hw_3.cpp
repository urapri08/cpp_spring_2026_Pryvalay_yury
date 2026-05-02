#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <memory>
#include <algorithm>

std::string remove_spaces(const std::string& s) {
  size_t start = s.find_first_not_of(" \n\r\t");
  if (start == std::string::npos) return "";
  size_t end = s.find_last_not_of(" \n\r\t");
  return s.substr(start, end - start + 1);
}

std::string text_to_file(const std::string& val) {
  std::string tmp;
  for (char ch : val) {
    if (ch == '\n') tmp += "\\n";
    else tmp += ch;
  }
  return tmp;
}

std::string file_to_text(const std::string& val) {
  std::string tmp;
  for (size_t k = 0; k < val.length(); ++k) {
    if (val[k] == '\\' && k + 1 < val.length() && val[k + 1] == 'n') {
      tmp += '\n';
      ++k;
    }
    else {
      tmp += val[k];
    }
  }
  return tmp;
}

void save(const std::map<std::string, std::string>& data, const std::string& path) {
  std::ofstream f(path);
  if (!f.is_open()) return;
  for (const std::pair<const std::string, std::string>& item : data) {
    f << item.first << "=" << text_to_file(item.second) << "\n";
  }
}

void process_file_data(std::map<std::string, std::string>& data, const std::string& path) {
  std::ifstream f(path);
  if (!f.is_open()) {
    std::cout << "Cannot open file: " << path << "\n";
    return;
  }

  int success_count = 0;
  int ignored_comments = 0;
  int bad_format = 0;
  std::string curr_line;

  while (std::getline(f, curr_line)) {
    std::string t_line = remove_spaces(curr_line);
    if (t_line.empty() || t_line[0] == '#') {
      ignored_comments++;
      continue;
    }

    size_t delim = curr_line.find('=');
    if (delim == std::string::npos) {
      bad_format++;
      continue;
    }

    std::string k = remove_spaces(curr_line.substr(0, delim));
    std::string v = remove_spaces(curr_line.substr(delim + 1));

    if (k.empty() || k.find("\\n") != std::string::npos) {
      bad_format++;
      continue;
    }

    data[k] = file_to_text(v);
    success_count++;
  }

  std::cout << "Parsing statistics for [" << path << "]:\n"
    << " Successfully loaded pairs: " << success_count << "\n"
    << " Skipped as comments: " << ignored_comments << "\n"
    << " Rejected due to format errors: " << bad_format << "\n\n";
}

void load(std::map<std::string, std::string>& data, const std::string& path) {
  data.clear();
  process_file_data(data, path);
}

void mergeFromFile(std::map<std::string, std::string>& data, const std::string& path) {
  process_file_data(data, path);
}

void print(const std::map<std::string, std::string>& data) {
  for (const std::pair<const std::string, std::string>& kv : data) {
    std::cout << kv.first << " = " << kv.second << "\n";
  }
  std::cout << "-----------------------\n";
}

void execute_task1() {
  std::cout << "=== TASK 1 ===\n";
  std::map<std::string, std::string> dict;

  dict["SystemUser"] = "Admin";
  dict["AccessLevel"] = "5";
  dict["WelcomeMessage"] = "Hello,\nAdministrator!";

  save(dict, "primary_config.txt");

  std::ofstream tmp_f("update_patch.txt");
  tmp_f << "# Update file for merging test\n";
  tmp_f << "AccessLevel = 6\n";
  tmp_f << "Encryption = Enabled\n";
  tmp_f << "Invalid\\nKey = 1\n";
  tmp_f << " = missing key test\n";
  tmp_f.close();

  std::map<std::string, std::string> application_settings;
  load(application_settings, "primary_config.txt");

  std::cout << "Dictionary contents after initial load:\n";
  print(application_settings);

  std::cout << "Executing mergeFromFile...\n";
  mergeFromFile(application_settings, "update_patch.txt");

  std::cout << "Final dictionary contents:\n";
  print(application_settings);
}

class Document {
  std::string title;
  std::string body;
public:
  Document(std::string t, std::string b) : title(std::move(t)), body(std::move(b)) {}
  const std::string& getTitle() const { return title; }
  const std::string& getBody() const { return body; }
  ~Document() {
    std::cout << "[Memory Cleared] Destroyed document: " << title << "\n";
  }
};

class DocumentSystem {
public:
  std::vector<std::unique_ptr<Document>> documents;
  const Document* current = nullptr;

  void add(const std::string& t, const std::string& b) {
    documents.push_back(std::make_unique<Document>(t, b));
  }

  void remove(size_t index) {
    if (index < documents.size()) {
      if (current == documents[index].get()) {
        current = nullptr;
      }
      documents.erase(documents.begin() + index);
    }
  }

  const Document* get(size_t index) const {
    if (index >= documents.size()) return nullptr;
    return documents[index].get();
  }

  const Document* findByTitle(const std::string& t) const {
    for (const std::unique_ptr<Document>& d : documents) {
      if (d->getTitle() == t) return d.get();
    }
    return nullptr;
  }

  bool removeByTitle(const std::string& t) {
    for (std::vector<std::unique_ptr<Document>>::iterator it = documents.begin(); it != documents.end(); ++it) {
      if ((*it)->getTitle() == t) {
        if (current == it->get()) {
          current = nullptr;
        }
        documents.erase(it);
        return true;
      }
    }
    return false;
  }

  void setCurrent(size_t idx) {
    current = get(idx);
  }

  void setCurrentByTitle(const std::string& t) {
    current = findByTitle(t);
  }
};

void execute_task2() {
  std::cout << "\n=== TASK 2 ===\n";
  DocumentSystem sys;
  sys.add("Research Proposal", "Content of the proposal");
  sys.add("Lab Report", "Content of the lab report");
  sys.add("Reference Manual", "Content of the manual");

  sys.setCurrent(1);
  std::cout << "Selected current document: " << sys.current->getTitle() << "\n";

  sys.remove(0);
  std::cout << "Removed the first element. The current pointer is still: ";
  if (sys.current != nullptr) {
    std::cout << sys.current->getTitle() << "\n";
  }
  else {
    std::cout << "nullptr\n";
  }

  sys.removeByTitle("Lab Report");
  std::cout << "Removed the active element itself. Now the pointer is: ";
  if (sys.current != nullptr) {
    std::cout << sys.current->getTitle() << "\n\n";
  }
  else {
    std::cout << "nullptr (working as intended)\n\n";
  }

  std::vector<std::unique_ptr<Document>> secondary_vector;
  std::cout << "Testing std::move: transferring a document to another vector...\n";

  secondary_vector.push_back(std::move(sys.documents[0]));
  sys.documents.erase(sys.documents.begin());

  std::cout << "Documents remaining in the original system: " << sys.documents.size() << "\n";
  std::cout << "Documents in the new vector: " << secondary_vector.size()
    << " (Title: " << secondary_vector[0]->getTitle() << ")\n";
}

struct Student {
  std::string name;
  std::map<std::string, double> grades;
};

double averageGrade(const Student& s) {
  if (s.grades.empty()) return -1.0;
  double total = 0.0;
  for (const std::pair<const std::string, double>& g : s.grades) {
    total += g.second;
  }
  return total / s.grades.size();
}

void display_students(const std::vector<Student>& v) {
  for (const Student& s : v) {
    double a = averageGrade(s);
    std::cout << " Student " << s.name << " \t| Average score: ";
    if (a == -1.0) {
      std::cout << "no grades\n";
    }
    else {
      std::cout << a << "\n";
    }
  }
  std::cout << "\n";
}

void execute_task3() {
  std::cout << "\n=== TASK 3 ===\n";
  std::vector<Student> lst = {
      {"Jetinovski", {{"Calculus", 5.0}, {"Physics", 4.0}}},
      {"Epstein", {{"Calculus", 3.0}}},
      {"Zimmerman", {}},
      {"Zeleboba", {{"Calculus", 4.0}, {"History", 4.0}}},
      {"Bykiv", {{"Computer Science", 2.0}}}
  };

  std::vector<Student> v1 = lst;
  std::sort(v1.begin(), v1.end(), [](const Student& s1, const Student& s2) {
    return s1.name < s2.name;
    });
  std::cout << "Step 1. Sorted strictly by name:\n";
  display_students(v1);

  std::vector<Student> v2 = lst;
  std::sort(v2.begin(), v2.end(), [](const Student& s1, const Student& s2) {
    double a1 = averageGrade(s1);
    double a2 = averageGrade(s2);
    if (a1 == -1.0 && a2 == -1.0) return false;
    if (a1 == -1.0) return false;
    if (a2 == -1.0) return true;
    return a1 > a2;
    });
  std::cout << "Step 2. Sorted by descending average grade:\n";
  display_students(v2);

  std::vector<Student> v3 = lst;
  std::sort(v3.begin(), v3.end(), [](const Student& s1, const Student& s2) {
    double a1 = averageGrade(s1);
    double a2 = averageGrade(s2);
    if (a1 == -1.0 && a2 == -1.0) return s1.name < s2.name;
    if (a1 == -1.0) return false;
    if (a2 == -1.0) return true;
    if (a1 == a2) return s1.name < s2.name;
    return a1 > a2;
    });
  std::cout << "Step 3. Descending average, fallback to name on equality:\n";
  display_students(v3);

  std::vector<Student> v4 = lst;
  std::stable_sort(v4.begin(), v4.end(), [](const Student& s1, const Student& s2) {
    return s1.name < s2.name;
    });
  std::cout << "Step 4. Executed stable_sort by name:\n";
  display_students(v4);

  std::vector<Student> v_part = lst;
  double minAvg = 4.0;
  std::vector<Student>::iterator iter = std::stable_partition(v_part.begin(), v_part.end(), [minAvg](const Student& s) {
    double a = averageGrade(s);
    return a >= minAvg && a != -1.0;
    });

  std::vector<Student> good_students(v_part.begin(), iter);
  std::vector<Student> poor_students(iter, v_part.end());

  std::cout << "[Partition Result] Satisfy the condition (>= 4.0):\n";
  display_students(good_students);
  std::cout << "[Partition Result] Do not satisfy the condition (or no grades):\n";
  display_students(poor_students);

  std::string subject = "Calculus";
  double minGrade = 4.0;
  std::vector<Student> filter_result;

  std::copy_if(lst.begin(), lst.end(), std::back_inserter(filter_result), [subject, minGrade](const Student& s) {
    std::map<std::string, double>::const_iterator f = s.grades.find(subject);
    if (f != s.grades.end()) {
      return f->second >= minGrade;
    }
    return false;
    });

  std::cout << "[Copy_if Result] Students with Calculus grade 4.0 or higher:\n";
  display_students(filter_result);
}

int main() {
  execute_task1();
  execute_task2();
  execute_task3();
  return 0;
}