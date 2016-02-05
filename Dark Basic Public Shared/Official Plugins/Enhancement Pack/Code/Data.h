
#ifndef CSPREADSHEET_H
#define CSPREADSHEET_H

#include "stdafx.h"
#include <odbcinst.h>
#include <afxdb.h>

class CSpreadSheet
{
public:
	CSpreadSheet(CString File, CString SheetOrSeparator, bool Backup = true); // Open spreadsheet for reading and writing
	~CSpreadSheet(); // Perform some cleanup functions
	bool AddHeaders(CStringArray &FieldNames, bool replace = false); // Add header row to spreadsheet
	bool DeleteSheet(); // Clear text delimited file content
	bool DeleteSheet(CString SheetName); // Clear entire Excel spreadsheet content. The sheet itself is not deleted
	bool AddRow(CStringArray &RowValues, long row = 0, bool replace = false); // Insert or replace a row into spreadsheet. Default is add new row. 
	bool AddCell(CString CellValue, CString column, long row = 0, bool Auto = true); // Replace or add a cell into Excel spreadsheet using header row or column alphabet. Default is add cell into new row. Set Auto to false if want to force column to be used as header name
	bool AddCell(CString CellValue, short column, long row = 0); // Replace or add a cell into spreadsheet using column number. Default is add cell into new row. 
	bool ReplaceRows(CStringArray &NewRowValues, CStringArray &OldRowValues); // Search and replace rows in Excel spreadsheet
	bool ReadRow(CStringArray &RowValues, long row = 0); // Read a row from spreadsheet. Default is read the next row
	bool ReadColumn(CStringArray &ColumnValues, CString column, bool Auto = true); // Read a column from Excel spreadsheet using header row or column alphabet. Set Auto to false if want to force column to be used as header name
	bool ReadColumn(CStringArray &ColumnValues, short column); // Read a column from spreadsheet using column number
	bool ReadCell (CString &CellValue, CString column, long row = 0, bool Auto = true); // Read a cell from Excel spreadsheet using header row or column alphabet. Default is read the next cell in next row. Set Auto to false if want to force column to be used as header name
	bool ReadCell (CString &CellValue, short column, long row = 0); // Read a cell from spreadsheet using column number. Default is read the next cell in next row.
	void BeginTransaction(); // Begin transaction
	bool Commit(); // Save changes to spreadsheet
	bool RollBack(); // Undo changes to spreadsheet
	bool Convert(CString SheetOrSeparator);
	inline void GetFieldNames (CStringArray &FieldNames) {FieldNames.RemoveAll(); FieldNames.Copy(m_aFieldNames);} // Get the header row from spreadsheet
	inline long GetTotalRows() {return m_dTotalRows;} // Get total number of rows in  spreadsheet
	inline short GetTotalColumns() {return m_dTotalColumns;} // Get total number of columns in  spreadsheet
	inline long GetCurrentRow() {return m_dCurrentRow;} // Get the currently selected row in  spreadsheet
	inline bool GetBackupStatus() {return m_bBackup;} // Get status of backup. True if backup is successful, False if spreadsheet is not backup
	inline bool GetTransactionStatus() {return m_bTransaction;} // Get status of Transaction. True if Transaction is started, False if Transaction is not started or has error in starting
	inline CString GetLastError() {return m_sLastError;} // Get last error message

private:
	bool Open(); // Open a text delimited file for reading or writing
	void GetExcelDriver(); // Get the name of the Excel-ODBC driver
	short CalculateColumnNumber(CString column, bool Auto); // Convert Excel column in alphabet into column number

	bool m_bAppend; // Internal flag to denote newly created spreadsheet or previously created spreadsheet
	bool m_bBackup; // Internal flag to denote status of Backup
	bool m_bExcel; // Internal flag to denote whether file is Excel spreadsheet or text delimited spreadsheet
	bool m_bTransaction; // Internal flag to denote status of Transaction

	long m_dCurrentRow; // Index of current row, starting from 1
	long m_dTotalRows; // Total number of rows in spreadsheet
	short m_dTotalColumns; // Total number of columns in Excel spreadsheet. Largest number of columns in text delimited spreadsheet

	CString m_sSql; // SQL statement to open Excel spreadsheet for reading
	CString m_sDsn; // DSN string to open Excel spreadsheet for reading and writing
	CString m_stempSql; // Temporary string for SQL statements or for use by functions
	CString m_stempString; // Temporary string for use by functions
	CString m_sSheetName; // Sheet name of Excel spreadsheet
	CString m_sExcelDriver; // Name of Excel Driver
	CString m_sFile; // Spreadsheet file name
	CString m_sSeparator; // Separator in text delimited spreadsheet
	CString m_sLastError; // Last error message

	CStringArray m_atempArray; // Temporary array for use by functions
	CStringArray m_aFieldNames; // Header row in spreadsheet
	CStringArray m_aRows; // Content of all the rows in spreadsheet

	CDatabase *m_Database; // Database variable for Excel spreadsheet
	CRecordset *m_rSheet; // Recordset for Excel spreadsheet
};
#endif