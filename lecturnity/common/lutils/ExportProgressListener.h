#pragma once

class ExportProgressListener
{
public:
    ExportProgressListener(void);
    ~ExportProgressListener(void);


    virtual void SetOverallProgress(int percent) = 0;
    virtual void SetOverallSteps(int steps) = 0;
    virtual void SetOverallLabel(LPCTSTR lpText) = 0;
    virtual void SetOverallLabel(UINT nResourceID) = 0;
    virtual void SetParticularProgress(int percent) = 0;
    virtual void SetParticularLabel(LPCTSTR lpText) = 0;
    virtual void SetParticularLabel(UINT nResourceID) = 0;
    virtual bool ExportWasCancelled() = 0;
};
