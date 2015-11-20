//
// Result Data
//

class CStructTable;

class CResultData
{
	public:
		// Normal Literal Data
		CStr*			m_pStringToken;

		// Optional Array Offset Data
		CStr*			m_pAdditionalOffset;

		// Breakdown of Data
		DWORD			m_dwType;
		DWORD			m_dwDataOffset;
		CStructTable*	m_pStruct;
};
