#include "chighlighter.h"

CHighlighter::CHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    // ========== 1. 关键字高亮 (紫色) ==========
    keywordFormat.setForeground(QColor("#C678DD"));
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywords = {
        "int", "float", "double", "char", "void", "return", "if", "else", "for", "while", "do",
        "switch", "case", "break", "continue", "struct", "union", "typedef", "static", "const",
        "goto", "sizeof", "volatile", "enum", "extern", "inline", "restrict", "default", "_Atomic",
        "_Bool", "_Complex", "_Generic", "_Imaginary", "_Noreturn", "_Static_assert", "_Thread_local"
    };
    for (const QString &keyword : keywords) {
        rules.append({QRegularExpression("\\b" + keyword + "\\b"), keywordFormat});
    }

    // ========== 2. 类型高亮 (青绿色) ==========
    typeFormat.setForeground(QColor("#56B6C2"));
    typeFormat.setFontItalic(true);
    QStringList types = {"FILE", "size_t", "ssize_t", "uint8_t", "uint16_t", "uint32_t", "uint64_t",
                         "int8_t", "int16_t", "int32_t", "int64_t", "time_t"};
    for (const QString &type : types) {
        rules.append({QRegularExpression("\\b" + type + "\\b"), typeFormat});
    }

    // ========== 3. 宏定义 (`#define`, `#include`) (红色) ==========
    macroFormat.setForeground(QColor("#E06C75"));
    macroFormat.setFontWeight(QFont::Bold);
    rules.append({QRegularExpression("^#\\s*(define|include|ifdef|ifndef|endif|pragma)\\b"), macroFormat});

    // ========== 4. 函数名高亮 (浅蓝色) ==========
    functionFormat.setForeground(QColor("#61AFEF"));
    functionFormat.setFontWeight(QFont::Bold);
    rules.append({QRegularExpression("\\b[A-Za-z_][A-Za-z0-9_]*\\s*(?=\\()"), functionFormat});

    // ========== 5. 数字高亮 (橙色) ==========
    numberFormat.setForeground(QColor("#D19A66"));
    rules.append({QRegularExpression("\\b[0-9]+\\b"), numberFormat});
    rules.append({QRegularExpression("\\b[0-9]+\\.[0-9]+\\b"), numberFormat});

    // ========== 6. 字符串高亮 (浅绿色) ==========
    stringFormat.setForeground(QColor("#98C379"));
    rules.append({QRegularExpression("\".*?\""), stringFormat});
    rules.append({QRegularExpression("'.*?'"), stringFormat});

    // ========== 7. 注释高亮 (灰色) ==========
    commentFormat.setForeground(QColor("#5C6370"));
    commentFormat.setFontItalic(true);
    rules.append({QRegularExpression("//[^\n]*"), commentFormat});

    // 多行注释
    multiLineCommentFormat.setForeground(QColor("#5C6370"));
    multiLineCommentFormat.setFontItalic(true);

    // ========== 8. 运算符高亮 (淡灰色) ==========
    operatorFormat.setForeground(QColor("#ABB2BF"));
    QString operators = R"([\+\-\*/%=<>&|^!~]+)";
    rules.append({QRegularExpression(operators), operatorFormat});
}

// 多行注释处理
void CHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightRule &rule : qAsConst(rules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // ========== 处理多行注释 ==========
    setCurrentBlockState(0);
    int startIndex = (previousBlockState() != 1) ? text.indexOf(QRegularExpression("/\\*")) : 0;
    while (startIndex >= 0) {
        int endIndex = text.indexOf(QRegularExpression("\\*/"), startIndex);
        int commentLength = (endIndex == -1) ? text.length() - startIndex : endIndex - startIndex + 2;
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        setCurrentBlockState((endIndex == -1) ? 1 : 0);
        startIndex = text.indexOf(QRegularExpression("/\\*"), startIndex + commentLength);
    }
}
