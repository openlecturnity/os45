#pragma once


// CRecordingProfileGallery
class CRecordingsToolBar;

class CRecordingProfileGallery : public CXTPControlGallery
{
public:
	CRecordingProfileGallery(CRecordingsToolBar *pToolbar);
	virtual ~CRecordingProfileGallery();

protected:
	virtual void OnMouseMove(CPoint point);
    afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);

private:
    CRecordingsToolBar *m_pToolbar;

};

class CRecordingProfileItem : public CXTPControlGalleryItem
{
public:
	CRecordingProfileItem();

    __int64 GetCustomProfileID() {return m_lCustomProfileID;}
    void SetCustomProfileID(__int64 lCustomProfileID) {m_lCustomProfileID = lCustomProfileID;}

public:
    __int64 m_lCustomProfileID;
};

