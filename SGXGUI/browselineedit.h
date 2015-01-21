///
/// SynGlyphX Holdings Incorporated ("COMPANY") CONFIDENTIAL
/// Copyright (c) 2013-2015 SynGlyphX Holdings Incorporated, All Rights Reserved.
///
/// NOTICE:  All information contained herein is, and remains the property of COMPANY. The intellectual and technical concepts contained
/// herein are proprietary to COMPANY and may be covered by U.S. and Foreign Patents, patents in process, and are protected by trade secret or copyright law.
/// Dissemination of this information or reproduction of this material is strictly forbidden unless prior written permission is obtained
/// from COMPANY.  Access to the source code contained herein is hereby forbidden to anyone except current COMPANY employees, managers or contractors who have executed 
/// Confidentiality and Non-disclosure agreements explicitly covering such access.
///
/// The copyright notice above does not evidence any actual or intended publication or disclosure  of  this source code, which includes  
/// information that is confidential and/or proprietary, and is a trade secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC  PERFORMANCE, 
/// OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS  SOURCE CODE  WITHOUT  THE EXPRESS WRITTEN CONSENT OF COMPANY IS STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE 
/// LAWS AND INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS  
/// TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.                
///

#ifndef BROWSELINEEDIT_H
#define BROWSELINEEDIT_H

#include "sgxgui_global.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

namespace SynGlyphX {

    class SGXGUI_EXPORT BrowseLineEdit : public QWidget
    {
        Q_OBJECT

    public:
        enum FileDialogType {
            FileOpen,
            FileSave,
            Directory
        };

        BrowseLineEdit(FileDialogType type, bool convertToNativeSeparators = true, QWidget *parent = 0);
        ~BrowseLineEdit();

        void SetFilters(const QString& filters);
        void SetInitalBrowseDirectory(const QString& dir);

        QString GetText() const;

    public slots:
        void SetText(const QString& text);

    signals:
        void TextChanged(QString text);

    private slots:
        void BrowseButtonActivated();
        void OnEditingFinished();

    private:
        FileDialogType m_dialogType;
        QLineEdit* m_lineEdit;
        QPushButton* m_browseButton;
        bool m_convertToNativeSeparators;
        QString m_filters;
        QString m_initialBrowseDirectory;
    };

} //namespace SynGlyphX

#endif // BROWSELINEEDIT_H
