Часть 1
 Рализованно - 
     1) Смена типа урона(1 - Лазер, 2 - Сфера)
     2) Урон лазером осуществляется зажатием ЛКМ (При этом режиме в отжатом состоянии выполняется заряда оружия)
     3) Урон сферой осуществляется при отпускании ЛКМ (Оружие должно быть предварительно заряжено (Зарядка осуществляется пока ЛКМ зажата))
     4) Переключение между режимами работает прокруткой колесика мыши
     5) Реализованные Акторы-Цели
       - DamagableCube - куб который способен получать урон
       - Turret - Туррель. Получает урон также как и куб. В неактивном состоянии бездействует, как только в область видимости попадает игрок- начинает ослеживать его 
         (Туррель концентрируется на первом попавшем в зону видимости игроке и наводится на  него пока тот не погибнет или не выйдет из зоны видимости.)
         Туррель обладает параметром TrackingSpeed - скорость наводки орудия. (Туррели с низкой скоростью трэкинга можно "закрутить")
       - Игрок 