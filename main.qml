import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts
import QtQuick.Controls

Window {
    width: 640
    height: 480
    maximumHeight: 480
    minimumHeight: 480
    maximumWidth: 640
    minimumWidth: 640
    visible: true
    title: qsTr("File encryptor")

    Dialog {
        id: instructions
        title: "Инструкции"
        standardButtons: Dialog.Ok
        Text {
            text: "В маске могут содержаться символы '*' и '?'\n* - сколько угодно символов\n? - какой-то один символ\nЕсли ввести конструкцию типа folder1/*/folder2, то это означает, что между каталогами folder1 и folder2\n может быть сколько угодно каталогов\nПримеры:\nC:/Users/HP/Documents/*.txt - все текстовые файлы в 'Документах'\nC:/Users/HP/Documents/*/F?.txt - все текстовые файлы, начинающиеся на F\nи заканчивающиеся любым символом. Поиск производится в 'Документах' и его подкаталогах"
        }
    }

    MenuBar {
        id: bar
        Menu {
            title: "О программе"
            Action {
                text: "Как оформлять маску файла?"
                onTriggered: instructions.open();
            }
        }
    }

    ColumnLayout {
        anchors {
            top: bar.bottom
            topMargin: 5
        }

        RowLayout {
            Layout.topMargin: 10
            Layout.leftMargin: 10

            Text {
                text: "Маска входного файла"
            }

            TextField {
                id: filemask
                objectName: "wildcard"
            }
        }

        CheckBox {
            id: checkbox
            objectName: "checkbox"
            Layout.leftMargin: 10
            text: "Удалять входные файлы"
        }

        RowLayout {
            Layout.leftMargin: 10
            Text {
                text: "Путь для сохранения результирующих файлов"
            }

            TextField {
                id: savepath
                objectName: "savepath"
                readOnly: true
            }

            Button {
                text: "..."
                onClicked: {
                    savepath.text = form.getDir();
                }
            }
        }

        RowLayout {
            Layout.leftMargin: 10
            Text {
                text: "Действия при повторении имени входного файла"
            }

            ComboBox {
                id: actionForRepeatedFiles
                objectName: "actionForRepeatedFiles"
                model: ["Перезапись", "Модификация"]
            }
        }

        RowLayout {
            Layout.leftMargin: 10
            Text {
                text: "Режим работы"
            }

            ComboBox {
                id: mode
                objectName: "mode"
                model: ["По таймеру", "Разовый запуск"]
            }
        }

        RowLayout {
            Layout.leftMargin: 10
            Text {
                text: "Периодичность опроса наличия входного файла (мс)"
            }

            TextField {
                id: timerperiod
                objectName: "timerperiod"
            }
        }

        RowLayout {
            Layout.leftMargin: 10
            Text {
                text: "Значение для модификации файла"
            }

            TextField {
                id: encryptionvalue
                objectName: "encryptionvalue"
            }
        }

        Button {
            Layout.leftMargin: 10
            Layout.topMargin: 20
            text: "Start"

            onClicked: {
                form.startEncryption();
            }
        }
    }
}
