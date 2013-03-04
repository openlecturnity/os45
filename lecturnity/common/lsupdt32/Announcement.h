#ifndef __ANNOUNCEMENTS_H__A63DC6CA_882B_492d_9A70_F0BEA03EF162
#define __ANNOUNCEMENTS_H__A63DC6CA_882B_492d_9A70_F0BEA03EF162

class CAnnouncement
{
public:
   CAnnouncement();
	CAnnouncement(CString csTitle, CString csUrl, bool bReleaseNote);

   CString m_csTitle;
   CString m_csUrl;
   
   bool    m_bReleaseNote;
   bool    m_bViewed;

   void SetViewed(bool bViewed = true) { m_bViewed = bViewed; }
   bool IsViewed() { return m_bViewed; }
};

#endif // __ANNOUNCEMENTS_H__A63DC6CA_882B_492d_9A70_F0BEA03EF162