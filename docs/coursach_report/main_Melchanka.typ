#import "../stp_typst_template_for_everyday_use/lib/stp2024.typ"

#show: stp2024.template

// Титульный лист
#include "011_titulnik_melchanka.typ"

#pagebreak()
// Содержание с 4-й страницы
#counter(page).update(4)
#stp2024.full_outline()

// Введение
#include "020_introduction.typ"

// Разделы Мельченко Д. А.
#include "030_architectura_vichislitelnoi_systemi.typ"
#include "040_platforma_programmnogo_obespechenia.typ"
#include "050_teoriticheskoye_obosnovaniye.typ"


#include "051_proektirovanie_melchanka.typ"
#include "061_architectura_melchanka.typ"

#pagebreak()
// Заключение
#include "070_conclusion.typ"

// Список источников
#bibliography("bibliography.bib")


