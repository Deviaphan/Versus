// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "HotkeyItemWidget.h"
#include "ui_HotkeyItemWidget.h"

HotkeyItemWidget::HotkeyItemWidget( QWidget* parent, const QKeySequence& inc, const QKeySequence& dec )
	: QWidget(parent)
	, ui(new Ui::HotkeyItemWidgetClass())
{
	ui->setupUi(this);

	connect( ui->btnClearDec, &QPushButton::clicked, this, [this]{ this->ui->seqDec->clear(); } );
	connect( ui->btnClearInc, &QPushButton::clicked, this, [this]{ this->ui->seqInc->clear(); } );

	ui->seqDec->setKeySequence( dec );
	ui->seqInc->setKeySequence( inc );
}

HotkeyItemWidget::~HotkeyItemWidget()
{
	delete ui;
}

void HotkeyItemWidget::SetTitle( int index )
{
	ui->label->setText( QString( "Player %1" ).arg( index + 1 ) );
}

QKeySequence HotkeyItemWidget::GetInc() const
{
	return ui->seqInc->keySequence();
}

QKeySequence HotkeyItemWidget::GetDec() const
{
	return ui->seqDec->keySequence();
}
