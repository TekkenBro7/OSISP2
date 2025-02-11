#!/bin/bash

# Проверяем, что переданы входные файлы, $# - количесвто аргументов
if [ "$#" -eq 0 ]; then
  echo "Использование: $0 файл1 [файл2 ...]"
  exit 1
fi

# Временный файл для хранения промежуточных данных
temp_file=$(mktemp)

# Объединяем все входные файлы в один, $@ - список всех аргументов
for file in "$@"; do
  if [ -f "$file" ]; then
    cat "$file" >> "$temp_file"
  else
    echo "Файл $file не существует."
  fi
done

# Проверяем, что временный файл не пуст
if [ ! -s "$temp_file" ]; then
  echo "Нет данных для обработки."
  rm -f "$temp_file"
  exit 1
fi

# Процессинг данных
awk -F, '
{
  name = $1; quantity = $2; price = $3;
  total_quantity[name] += quantity;
  total_price[name] += price * quantity;
  count[name]++;
}
END {
  printf "%-20s %-10s %-10s %-10s\n", "Наименование", "Кол-во", "Сред. цена", "Общ. стоимость";
  for (name in total_quantity) {
    avg_price = total_price[name] / total_quantity[name];
    printf "%-20s %-10d %-10.2f %-10.2f\n", name, total_quantity[name], avg_price, total_price[name];
  }
}' "$temp_file"

# Удаляем временный файл
rm -f "$temp_file"