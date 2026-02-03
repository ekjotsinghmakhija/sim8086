#include "syntax_highlighter.h"

AsmHighlighter::AsmHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    // 1. Opcodes (Blue, Bold)
    keywordFormat.setForeground(QColor("#569CD6")); // VS Code Blue
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywords = {
        "MOV", "ADD", "SUB", "JMP", "HLT", "MUL", "DIV", "INC", "DEC",
        "PUSH", "POP", "CALL", "RET", "INT", "CMP", "JE", "JNE"
    };
    for (const QString &keyword : keywords) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression("\\b" + keyword + "\\b", QRegularExpression::CaseInsensitiveOption);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // 2. Registers (Orange)
    registerFormat.setForeground(QColor("#CE9178")); // VS Code Orange
    QStringList regs = {
        "AX", "BX", "CX", "DX", "AH", "AL", "BH", "BL", "CH", "CL", "DH", "DL",
        "CS", "DS", "ES", "SS", "SI", "DI", "SP", "BP", "IP"
    };
    for (const QString &reg : regs) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression("\\b" + reg + "\\b", QRegularExpression::CaseInsensitiveOption);
        rule.format = registerFormat;
        highlightingRules.append(rule);
    }

    // 3. Numbers/Hex (Mint Green)
    numberFormat.setForeground(QColor("#B5CEA8"));
    HighlightingRule numberRule;
    // Matches 1234, 0x1234, 1234h
    numberRule.pattern = QRegularExpression("\\b[0-9A-Fa-f]+h?\\b");
    numberRule.format = numberFormat;
    highlightingRules.append(numberRule);

    // 4. Labels (Yellow)
    labelFormat.setForeground(QColor("#DCDCAA"));
    HighlightingRule labelRule;
    labelRule.pattern = QRegularExpression("^[A-Za-z_][A-Za-z0-9_]*:");
    labelRule.format = labelFormat;
    highlightingRules.append(labelRule);

    // 5. Comments (Grey) - MUST BE LAST
    commentFormat.setForeground(QColor("#6A9955"));
    HighlightingRule commentRule;
    commentRule.pattern = QRegularExpression(";[^\n]*");
    commentRule.format = commentFormat;
    highlightingRules.append(commentRule);
}

void AsmHighlighter::highlightBlock(const QString &text) {
    for (const HighlightingRule &rule : highlightingRules) {
        QRegularExpressionMatchIterator i = rule.pattern.globalMatch(text);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
