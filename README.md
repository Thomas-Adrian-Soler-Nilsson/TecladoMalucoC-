# 🎹 MyAplicationInKeyboard

> 🌀 Um pequeno experimento em C++ que intercepta as teclas do teclado e inverte as letras digitadas em tempo real — A ↔ Z, B ↔ Y, C ↔ X... um verdadeiro *teclado maluco*. 😈  

---

## 💡 Descrição

O projeto **MyAplicationInKeyboard** utiliza *Windows Hooks* (`WH_KEYBOARD_LL`) para capturar os eventos do teclado globalmente no sistema e aplicar uma lógica divertida: cada letra digitada é substituída pela sua “inversa” no alfabeto.

Por exemplo:
A → Z
B → Y
C → X
Z → A

yaml
Copiar código

Além disso:
- Pressionar **ESC** encerra o programa com segurança.  
- Todas as teclas não alfabéticas são ignoradas (mantêm o comportamento normal).  

---

## 🧠 Tecnologias usadas

- 💻 **C++ (WinAPI)**
- 🧩 **Windows Hooks**
- 🧰 **Dev-C++ 5.11**
- ⚙️ `windows.h`, `ctype.h`, `SendInput`, `ToUnicode`, `KBDLLHOOKSTRUCT`

---

## 🧩 Estrutura do projeto
---

## ⚙️ Como compilar no Dev-C++

1. Abra o **Dev-C++**
2. Vá em **Arquivo → Abrir Projeto**
3. Selecione `MyAplicationInKeyboard.dev`
4. Clique em **Executar → Compilar e Executar (F11)**  
5. Permita a execução (pode precisar de permissão de administrador 👑)

> 💬 Quando estiver rodando, tudo que você digitar será invertido... cuidado com as senhas 😏

---

## 🧨 Encerrando o programa

Pressione **ESC** a qualquer momento para encerrar o hook e finalizar o programa.

---

## ⚠️ Aviso

Este código é apenas para **fins educacionais**.  
Evite usá-lo em sistemas de produção ou para interceptar teclas de terceiros.

---

✨ *“Nem toda loucura é perigosa… algumas apenas invertem letras.”* 😌
