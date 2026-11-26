# mdeditor
Simple markdown editor. Written by AI.

Кроссплатформенный редактор текста в разметке Markdown для Linux, Windows и
macOS: слева текст редактируется, справа сразу видно, что из него получается.

Поддерживается CommonMark, а также расширения GitHub Flavored Markdown:
таблицы, списки задач, зачёркивание и автоматические ссылки (в том числе
адреса без угловых скобок). Конвертер проверяется на 673 официальных примерах
этих спецификаций (см. «Тесты» ниже).

## Возможности

- Создание, открытие и сохранение файлов Markdown и обычного текста
  (UTF-8, без метки порядка байтов); сохранение под другим именем.
- Предпросмотр, который обновляется при наборе текста.
- Экспорт отрендеренного документа:
  - в самостоятельный HTML-файл со встроенной таблицей стилей;
  - в PDF (страницы A4, поля около двух сантиметров, номера страниц).
- Картинки с локальными путями показываются в предпросмотре и попадают в PDF;
  при экспорте в HTML пути картинок пересчитываются относительно места, куда
  сохраняется страница.
- Ссылки открываются системным браузером; ссылки на соседние файлы работают.
- Запрос о сохранении изменений при закрытии окна, создании и открытии
  другого документа.
- Масштаб предпросмотра (Ctrl+колесо мыши, Ctrl++/Ctrl+-/Ctrl+0).
- Отступ текста по Tab и Shift+Tab.
- Позиция курсора в строке состояния; файл можно передать аргументом командной
  строки (`mdeditor заметка.md`).
- Положение и размер окна запоминаются между запусками.

## Сборка и запуск

Требуются CMake 3.16 или новее, компилятор с поддержкой C++17 и Qt 5.15
или новее (модуль Widgets; на Linux дополнительно пакеты разработки Qt,
например `qtbase5-dev` в Debian/Ubuntu, `qt5-base` в Arch, `qt5-qtbase-devel`
в Fedora).

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
./build/mdeditor
```

Для Visual Studio и Xcode исполняемый файл находится в подкаталоге конфигурации
сборки (`build/Release/mdeditor.exe`, `build/mdeditor.app/Contents/MacOS/mdeditor`).
Установка: `cmake --install build --prefix КАТАЛОГ`.

## Тесты

```sh
ctest --test-dir build --output-on-failure
```

Четыре набора проверяют конвертер Markdown, работу с файлами и оба экспорта,
документ предпросмотра и окно программы — всего более 800 проверок. Наборы
запускаются без экрана (`QT_QPA_PLATFORM=offscreen`), поэтому годятся для
серверов сборки.

Файл `tests/spec_examples.inc` сгенерирован из официальных примеров
спецификаций CommonMark и GitHub Flavored Markdown (оба текста
распространяются по лицензии CC BY-SA 4.0, как и этот файл). Примеры,
отличающиеся из-за автоматических ссылок без угловых скобок, исключены.

Перегенерировать файл можно так:

```sh
cd tests/tools
./build_mdtest.sh ~/programm/mdeditor/src
curl -o /tmp/commonmark-spec.txt https://raw.githubusercontent.com/commonmark/commonmark-spec/master/spec.txt
curl -o /tmp/gfm-spec.txt https://raw.githubusercontent.com/github/cmark-gfm/master/test/spec.txt
python3 make_spec_examples.py /tmp/commonmark-spec.txt /tmp/gfm-spec.txt ./mdtest ../spec_examples.inc
```

## Структура проекта

```
CMakeLists.txt          сборка: библиотека ядра, библиотека интерфейса, программа, тесты
src/main.cpp            точка входа
src/core/               ядро без интерфейса:
  markdown_*.cpp|h      конвертер Markdown в HTML (блоки, строки, таблицы, ссылки)
  html_entities.inc     таблица именованных символов HTML5 (сгенерирована)
  html_page.cpp|h       HTML-страница и таблицы стилей
  rich_text_document.*  документ для предпросмотра и печати, картинки
  document_io.*         файлы Markdown, экспорт в HTML и PDF
src/gui/                окна и панели: редактор, предпросмотр, главное окно
tests/                  наборы тестов Qt Test и данные спецификаций
```
