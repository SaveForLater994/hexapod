#import "../stp_typst_template_for_everyday_use/lib/stp2024.typ"

#show: stp2024.template

// Титульный лист
#include "010_titulnik_Kushnerov.typ"

#pagebreak()
// Содержание с 4-й страницы
#counter(page).update(4)
#stp2024.full_outline()

// Введение
#include "020_introduction.typ"

// Разделы Кушнерова А. А.
#include "030_architectura_vichislitelnoi_systemi.typ"
#include "040_platforma_programmnogo_obespechenia.typ"
#include "050_proektirovanie_kushnerov.typ"
#include "060_architectura_kushnerov.typ"

// Заключение
#include "070_conclusion.typ"

// Список источников
#bibliography("bibliography.bib")


