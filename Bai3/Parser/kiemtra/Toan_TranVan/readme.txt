

 1. XÓA CÁC THÔNG BÁO DEBUG (Yêu cầu 1)

Xóa tất cả các dòng `assert()` để chỉ in ra file nguồn và báo lỗi.
Các file thay đổi:
-   `parser.c`: Xóa tất cả các dòng `assert()` trong các hàm:
    -   `compileProgram()`
    -   `compileBlock()`
    -   `compileSubDecls()`
    -   `compileFuncDecl()`
    -   `compileProcDecl()`
    -   `compileAssignSt()`
    -   `compileCallSt()`
    -   `compileGroupSt()`
    -   `compileIfSt()`
    -   `compileWhileSt()`
    -   `compileForSt()`
    -   `compileExpression()`


2. KHAI BÁO NHIỀU BIẾN VÀ KHỞI TẠO GIÁ TRỊ (Yêu cầu 2)
Cho phép khai báo nhiều biến cùng kiểu trên một dòng và có thể khởi tạo giá trị cho biến ngay khi khai báo.
Cú pháp mới:
Var x, y = 20, z : integer;
Các file thay đổi:
`parser.c` - Hàm `compileVarDecl()`:
    -   Parse tên biến đầu tiên
    -   Kiểm tra có khởi tạo giá trị không (dấu `=`)
    -   Lặp qua các biến tiếp theo (phân cách bởi dấu `,`)
    -   Mỗi biến có thể có khởi tạo giá trị riêng
    -   Cuối cùng là dấu `:` và kiểu dữ liệu chung

3. CẤU TRÚC LỆNH FOR KIỂU C (Yêu cầu 3)

Sửa lại cấu trúc lệnh FOR theo cú pháp của C. Cú pháp cũ vẫn được chấp nhận.
Cú pháp mới:
For(n := 10; n < 100; n := n + 1) S := S + n;
Cú pháp cũ (vẫn hoạt động):
For n := 1 To 10 Do S := S + n;
Các file thay đổi:

-   `parser.c` - Hàm `compileForSt()`: (dòng 391)
    -   Kiểm tra token tiếp theo sau `FOR`
    -   Nếu là `(` → Cú pháp kiểu C:
        -   Parse phần khởi tạo: `n := 10`
        -   Parse dấu `;`
        -   Parse điều kiện: `n < 100`
        -   Parse dấu `;`
        -   Parse phần cập nhật: `n := n + 1`
        -   Parse dấu `)`
        -   Parse statement
    -   Nếu không phải `(` → Cú pháp cũ (Pascal):
        -   Parse theo cú pháp: `identifier := expr TO expr DO statement`

4. BIỂU THỨC ĐIỀU KIỆN (TERNARY OPERATOR) (Yêu cầu 4)

Chấp nhận biểu thức điều kiện tương tự C (toán tử `? :`).
Cú pháp:
X := a > b ? a = a + 5 : b = b * 2

Các file thay đổi:

1. token.h: Thêm `SB_QUESTION` vào enum `TokenType`
    SB_LPAR, SB_RPAR, SB_LSEL, SB_RSEL, SB_QUESTION
2. charcode.h: Thêm `CHAR_QUESTION` vào enum `CharCode`
    CHAR_LPAR, CHAR_RPAR, CHAR_QUESTION, CHAR_UNKNOWN
3. charcode.c: Gán CHAR_QUESTION cho ký tự '?' (ASCII 63)
    - Thay đổi phần tử cuối dòng thứ 4 của mảng từ `CHAR_UNKNOWN` thành `CHAR_QUESTION`
4. scanner.c:
    - Thêm case `CHAR_QUESTION` trong hàm `getToken()`:
        case CHAR_QUESTION:
          token = makeToken(SB_QUESTION, lineNo, colNo);
          readChar();
          return token;
    - Thêm case `SB_QUESTION` trong hàm `printToken()`:
        case SB_QUESTION: printf("SB_QUESTION\n"); break;
5. **token.c**: Thêm SB_QUESTION vào hàm `tokenToString()`
    case SB_QUESTION: return "\'?\'";
6. **parser.c** - Hàm `compileExpression()`:
    - Sau khi parse expression bình thường
    - Kiểm tra nếu token tiếp theo là `?`
    - Parse expression cho trường hợp đúng
    - Parse dấu `:`
    - Parse expression cho trường hợp sai
