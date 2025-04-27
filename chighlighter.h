#ifndef CHIGHLIGHTER_H
#define CHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

class CHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit CHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QVector<HighlightRule> rules;

    QTextCharFormat keywordFormat;
    QTextCharFormat typeFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat macroFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat stringFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat operatorFormat;
};

#endif // CHIGHLIGHTER_H
