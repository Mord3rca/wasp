#include "csv.hpp"

CSVRow::CSVRow( std::istream &file, char delim)
{
  this->readRow(file, delim);
}

std::size_t CSVRow::size() const
{
  return m_data.size();
}

const std::string& CSVRow::operator [](std::size_t index) const
{
  return m_data[index];
}

void CSVRow::readRow( std::istream& file, char delimiter)
{
  std::string line, cell;
  std::getline( file, line );
  
  std::istringstream lineStream(line);
  
  m_data.clear();
  while( std::getline(lineStream, cell, delimiter) && lineStream)
    m_data.push_back(cell);
}

CSV::CSV() : m_ptr_file(nullptr), m_delimiter(',')
{}

CSV::CSV( std::istream &file, char delim) : m_ptr_file( file.good() ? &file : nullptr ),
                                            m_delimiter(delim)
{}

void CSV::ReadAll()
{
  if( m_ptr_file == nullptr )
    return;
  
  while(m_ptr_file->good())
  {
    CSVRow* tmp = new CSVRow(*m_ptr_file, m_delimiter);
    m_rows.push_back(tmp);
  }
}

CSV::~CSV()
{
  for(auto i : m_rows)
    delete i;
}

const CSVRow& CSV::operator []( std::size_t index) const
{
  return *m_rows[index];
}

const std::size_t CSV::size() const
{
  return m_rows.size();
}
