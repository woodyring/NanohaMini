/*
  NanohaMini, a USI shogi(japanese-chess) playing engine derived from Stockfish 2.1
  Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
  Copyright (C) 2008-2010 Marco Costalba, Joona Kiiski, Tord Romstad (Stockfish author)
  Copyright (C) 2014 Kazuyuki Kawabata

  NanohaMini is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  NanohaMini is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <cctype>
#include <iostream>
#include <sstream>
#include "misc.h"
#include "thread.h"
#include "ucioption.h"

using std::string;
using std::cout;
using std::endl;

OptionsMap Options; // Global object


// Our case insensitive less() function as required by UCI protocol
bool CaseInsensitiveLess::operator() (const string& s1, const string& s2) const {

  int c1, c2;
  size_t i = 0;

  while (i < s1.size() && i < s2.size())
  {
      c1 = tolower(s1[i]);
      c2 = tolower(s2[i++]);

      if (c1 != c2)
          return c1 < c2;
  }
  return s1.size() < s2.size();
}


// stringify() converts a numeric value of type T to a std::string
template<typename T>
static string stringify(const T& v) {

  std::ostringstream ss;
  ss << v;
  return ss.str();
}


/// OptionsMap c'tor initializes the UCI options to their hard coded default
/// values and initializes the default value of "Threads" and "Minimum Split Depth"
/// parameters according to the number of CPU cores.

OptionsMap::OptionsMap() {

  OptionsMap& o = *this;

  // ���
  o["OwnBook"] = UCIOption(true);
  o["RandomBookSelect"] = UCIOption(true);
#if defined(NANOHA)
  o["BookFile"] = UCIOption("book_40.jsk");
#else
  o["BookFile"] = UCIOption("book.bin");
#endif
#if defined(NANOHA)
  o["Ponder"] = UCIOption(false);
#else
  o["Ponder"] = UCIOption(true);
#endif
  o["Threads"] = UCIOption(1, 1, MAX_THREADS);
  o["Hash"] = UCIOption(256, 4, 1024);

  o["Use Search Log"] = UCIOption(false);
  o["Search Log Filename"] = UCIOption("SearchLog.txt");
  o["Minimum Split Depth"] = UCIOption(4, 4, 7);
  o["Maximum Number of Threads per Split Point"] = UCIOption(5, 4, 8);
  o["Use Sleeping Threads"] = UCIOption(false);
  o["Clear Hash"] = UCIOption(false, "button");
  o["MultiPV"] = UCIOption(1, 1, 500);
  o["Skill Level"] = UCIOption(20, 0, 20);
#if defined(NANOHA)
  // TODO: �v�����G���Ԑ���̃p�����[�^
  o["Emergency Move Horizon"] = UCIOption(50, 0, 60);
  o["Emergency Base Time"] = UCIOption(20000, 0, 30000);
  o["Emergency Move Time"] = UCIOption(1000, 0, 5000);
#else
  o["Emergency Move Horizon"] = UCIOption(40, 0, 50);
  o["Emergency Base Time"] = UCIOption(200, 0, 30000);
  o["Emergency Move Time"] = UCIOption(70, 0, 5000);
#endif
  o["Minimum Thinking Time"] = UCIOption(20, 0, 5000);
#if defined(NANOHA)
  // ��������(�����)�̓_��
  o["DrawValue"] = UCIOption(0, -30000, 30000);
  o["Output_AllDepth"] = UCIOption(false);
#endif

  // Set some SMP parameters accordingly to the detected CPU count
  UCIOption& thr = o["Threads"];
  UCIOption& msd = o["Minimum Split Depth"];

  // �X���b�h��
  int thr_num = Min(cpu_count(), MAX_THREADS);
  thr.defaultValue = thr.currentValue = stringify(thr_num);

  if (cpu_count() >= 8)
      msd.defaultValue = msd.currentValue = stringify(7);
}


/// OptionsMap::print_all() returns a string with all the UCI options in chronological
/// insertion order (the idx field) and in the format defined by the UCI protocol.

string OptionsMap::print_all() const {

  std::stringstream s;

  for (size_t i = 0; i <= size(); i++)
      for (OptionsMap::const_iterator it = begin(); it != end(); ++it)
          if (it->second.idx == i && (it->first.find(" ") == string::npos))
          {
              const UCIOption& o = it->second;
              s << "\noption name " << it->first << " type " << o.type;

              if (o.type != "button")
                  s << " default " << o.defaultValue;

              if (o.type == "spin")
                  s << " min " << o.minValue << " max " << o.maxValue;

              break;
          }
  return s.str();
}


/// Option class c'tors

UCIOption::UCIOption(const char* def) : type("string"), minValue(0), maxValue(0), idx(Options.size())
{ defaultValue = currentValue = def; }

UCIOption::UCIOption(bool def, string t) : type(t), minValue(0), maxValue(0), idx(Options.size())
{ defaultValue = currentValue = (def ? "true" : "false"); }

UCIOption::UCIOption(int def, int minv, int maxv) : type("spin"), minValue(minv), maxValue(maxv), idx(Options.size())
{ defaultValue = currentValue = stringify(def); }


/// set_value() updates currentValue of the Option object. Normally it's up to
/// the GUI to check for option's limits, but we could receive the new value
/// directly from the user by teminal window. So let's check the bounds anyway.

void UCIOption::set_value(const string& v) {

  assert(!type.empty());

  if (v.empty())
      return;

  if ((type == "check" || type == "button") != (v == "true" || v == "false"))
      return;

  if (type == "spin")
  {
      int val = atoi(v.c_str());
      if (val < minValue || val > maxValue)
          return;
  }

  currentValue = v;
}
