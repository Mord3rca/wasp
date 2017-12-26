#ifndef CSV_HPP
#define CSV_HPP

#include <istream>
#include <sstream>
#include <vector>
#include <string>

class CSVRow
{
public:
  CSVRow( std::istream&, char = ',');
  
  const std::string& operator [](std::size_t) const;
  std::size_t size() const;
  
  void readRow( std::istream&, char );

private:
  std::vector< std::string > m_data;
};

class CSV
{
public:
  CSV();
  CSV(std::istream&, char = ',');
  
  ~CSV();
  
  const CSVRow& operator [](std::size_t) const;
  const std::size_t size() const;
  
  void ReadAll();
  
private:
  std::istream  *m_ptr_file;
  char          m_delimiter;
  
  std::vector< CSVRow* > m_rows;
};

#endif //CSV_HPP
