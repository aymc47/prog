# 命名規則・API設計規約（Queue / WaitQueue / Event）

本書は、本プロジェクトにおける **命名規則** および **API設計規約** を定義するものである。  
C 言語（pthread / 組込み指向）で実装される Queue / WaitQueue / Event 系モジュールを対象とする。

---

## 1. 基本方針

- C 言語であっても **擬似オブジェクト指向** を採用する
- 型・関数・引数・戻り値の役割が **名前だけで分かる** ことを重視する
- 割り込み文脈（ISR）と通常文脈を **命名で明確に分離** する
- 将来的な RTOS / FreeRTOS / AUTOSAR 置換を考慮する

---

## 2. 型名（typedef / struct）

### 2.1 命名規則

- 型名は **UpperCamelCase** を用いる
- `typedef struct { ... } TypeName;` 形式を基本とする

```c
typedef struct {
    ...
} WaitQueueISR;
```

### 2.2 ISR 専用型

- 割り込み文脈専用の型は **サフィックス `ISR`** を付与する

例：
- `WaitQueueISR`
- `WaitEventISR`

---

## 3. 構造体メンバ名

### 3.1 命名規則

- 構造体メンバは必ず **`m_` プレフィックス** を付ける
- 以降は **lower_snake_case** を用いる

```c
typedef struct {
    Queue           m_queue;
    pthread_cond_t  m_cond;
    pthread_mutex_t m_mutex;
} WaitQueueISR;
```

### 3.2 意味

| 接頭辞 | 意味 |
|------|------|
| `m_` | member（構造体のメンバ） |

---

## 4. 関数名（公開 API）

### 4.1 基本構造

```text
lib_<module>_<action>[_<qualifier>]
```

例：
- `lib_queue_init`
- `lib_wait_queue_push`
- `lib_event_wakeup_from_isr`

### 4.2 プレフィックス `lib_`

- 外部公開 API であることを示す
- 内部関数（static）は `lib_` を付けない

---

## 5. モジュール名（module）

- 対象概念を **lower_snake_case** で表す

例：
- `queue`
- `wait_queue`
- `event`

---

## 6. 操作名（action）

| 動詞 | 意味 |
|----|----|
| `init` | 初期化 |
| `push` | 追加 |
| `pop` | 取り出し（削除を伴う） |
| `get` | 取得（削除しない） |
| `remove` | 指定要素の削除 |
| `wait` | 待機 |
| `wakeup` | 起床通知 |

---

## 7. 修飾子（qualifier）

### 7.1 ISR 関連

| 修飾子 | 意味 |
|------|------|
| `_isr` | ISR 文脈で実行される関数 |
| `_from_isr` | ISR から呼ばれる関数 |

例：
- `lib_event_wait_isr`
- `lib_event_wakeup_from_isr`

---

## 8. 引数命名規則

### 8.1 方向プレフィックス

| プレフィックス | 意味 |
|-------------|------|
| `in_` | 入力専用 |
| `out_` | 出力専用 |
| `inout_` | 入出力 |

### 8.2 ポインタ明示

- ポインタ型引数は **必ず `_ptr` サフィックス** を付与する

```c
Queue* in_ins_ptr
Node*  in_node_ptr
int*   out_rc_ptr
```

---

## 9. 戻り値規約（Return Code Rule）

### 9.1 基本方針

- **操作の成否** は `int` で返す
- **取得対象そのもの** はポインタで返す
- 詳細エラーは `out_rc_ptr` で返す

### 9.2 戻り値の意味

| 値 | 意味 |
|----|----|
| `0` | 成功 |
| `<0` | 失敗（独自エラー） |
| `>0` | `errno` 準拠エラーコード |

### 9.3 例

```c
Node* lib_queue_pop(Queue* in_ins_ptr, int* out_rc_ptr);
```

- 成功時：戻り値 = `Node*`, `*out_rc_ptr = 0`
- 失敗時：戻り値 = `NULL`, `*out_rc_ptr = errno`

---

## 10. in / out の所有権ルール（Ownership Rule）

### 10.1 入力引数（in_）

- 呼び出し側が所有権を持つ
- 関数は **解放・保持しない**

### 10.2 出力引数（out_）

- 呼び出し側がメモリを用意する
- 関数は **値を書き込むのみ**

### 10.3 返却ポインタ

- 所有権の移動が発生する場合は **API仕様に明記**する
- Queue から `pop` された `Node*` の所有権は呼び出し側に移る

---

## 11. 禁止事項

- 意味不明な略語の使用
- `_ptr` のないポインタ引数
- 戻り値と out 引数の役割が重複する設計
- ISR 文脈で pthread mutex / cond を直接操作すること

---

## 12. 略語辞書（共通）

| 略語 | 意味 |
|----|----|
| `ins` | instance |
| `rc` | return code |
| `ptr` | pointer |
| `isr` | interrupt service routine |

---

## 13. 備考

本規約は以下を前提として設計されている。

- pthread 実装 → 将来的に RTOS API へ置換可能
- AUTOSAR / MISRA-C 観点でのレビュー容易性
- 静的解析ツール（clang-tidy 等）との親和性

---

（End of Document）

