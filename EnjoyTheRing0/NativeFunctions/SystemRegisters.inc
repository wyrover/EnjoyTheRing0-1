; ���������� cr0.wp (16� ���, ������� � ��������):
proc DisableWriteProtection
  if defined x64 
    mov rax, cr0
    btr rax, 16
    mov cr0, rax
  else
    mov eax, cr0
    btr eax, 16
    mov cr0, eax  
  end if
  
  ret
endp

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; ������� cr0.wp (16� ���, ������� � ��������):
proc EnableWriteProtection
  if defined x64 
    mov rax, cr0
    bts rax, 16
    mov cr0, rax
  else
    mov eax, cr0
    bts eax, 16
    mov cr0, eax  
  end if
  
  ret
endp

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; SMEP = 7� ��� (������� � ��������) � EBX ����� CPUID (EAX = 7, ECX = 0):
proc IsSMEPPresent
  if defined x64
    push rbx
    push rcx
    push rdx
  else
    push ebx
    push ecx
    push edx
  end if
  
  mov eax, 7
  xor ecx, ecx
  cpuid
  xor eax, eax
  mov cx, 0FFFFh
  and ebx, 80h
  cmovnz ax, cx
  
  if defined x64
    pop rdx
    pop rcx
    pop rbx
  else
    pop edx
    pop ecx
    pop ebx
  end if
  
  ret
endp

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; ���������� cr4.smep (20� ���, ������� � ��������):
proc DisableSMEP
  if defined x64 
    mov rax, cr4
    btr rax, 20
    mov cr4, rax
  else
    mov eax, cr4
    btr eax, 20
    mov cr4, eax  
  end if

  ret
endp

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; ������� cr4.smep (20� ���, ������� � ��������):
proc EnableSMEP
  if defined x64 
    mov rax, cr4
    bts rax, 20
    mov cr4, rax
  else
    mov eax, cr4
    bts eax, 20
    mov cr4, eax  
  end if

  ret
endp

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; SMAP = 20� ��� (������� � ��������) � EBX ����� CPUID (EAX = 7, ECX = 0):
proc IsSMAPPresent
  if defined x64
    push rbx
    push rcx
    push rdx
  else
    push ebx
    push ecx
    push edx
  end if
  
  mov eax, 7
  xor ecx, ecx
  cpuid
  xor eax, eax
  mov cx, 0FFFFh
  and ebx, 100000h
  cmovnz ax, cx
  
  if defined x64
    pop rdx
    pop rcx
    pop rbx
  else
    pop edx
    pop ecx
    pop ebx
  end if
  
  ret
endp

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; ���������� cr4.smap (21� ���, ������� � ��������):
proc DisableSMAP
  if defined x64 
    mov rax, cr4
    btr rax, 21
    mov cr4, rax
  else
    mov eax, cr4
    btr eax, 21
    mov cr4, eax  
  end if

  ret
endp

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; ������� cr4.smap (21� ���, ������� � ��������):
proc EnableSMAP
  if defined x64 
    mov rax, cr4
    bts rax, 21
    mov cr4, rax
  else
    mov eax, cr4
    bts eax, 21
    mov cr4, eax  
  end if

  ret
endp

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

proc OperateCrDrRegister ; OperationType: Word, OptionalData: LongWord|Uint64
; CH  = NN - ����� �������� [rCR:20h, wCR:22h, rDR:21h, wDR:23h]
; CL  = XX - ����� �������� [0..15]
; rDX = ������������ ������ ��� ������ (���� CH = 22h ��� 23h)
    
; XX - ����� ��������: [[C0, C8, D0, D8, E0, E8, F0, F8]]
; NN - �������� (r/w) � ������� (CR/DR) : [[rCR:20h, wCR:22h, rDR:21h, wDR:23h]]
    
;    0F NN XX | x86|x64 | mov rAX, xR0..xR7  | mov xR0..xR7 , rAX
; F0 0F NN C0 |     x86 | mov EAX, xR8..xR15 | mov xR8..xR15, EAX
; 44 0F NN XX |     x64 | mov RAX, xR8..xR15 | mov xR8..xR15, RAX

; ��������� NN �� ����������:
@ActionChecking:
  mov eax, 23212220h ; ���������� ��� ��������: 20h, 22h, 21h � 23h
  cmp ch, al
  je @ValidationSuccessful
  shr eax, 8
  test eax, eax
  jz @ValidationFailed
  
@ValidationSuccessful:
  
  ; ��������� �� ���������� ����� ��������
  cmp cl, 15
  jg @ValidationFailed ; ���� ����� �������� > 15 - ���������� �����
  
  cmp cl, 7
  jg @NumberIsGreaterThan7 ; ���� ����� [8..15] - �������������� �������� ���

  ; ����������� ����, ������������ ����� ��������:
  mov al, cl
  mov cl, 8 ; �����, ������������ ����� ��������, ���� �� ��������� 8
  mul cl
  mov cl, al
  add cl, 0C0h ; CL = 0xC0 + ByteNumber * 8 - ����, ������������ ����� ��������
  
  ; ��������� ��������������!
  mov ax, 900Fh ; � CH - ����� ��������, � AH - NOP
  xchg ah, ch   ; �������� �� �������: � CH - NOP, � AH - ����� ��������
  
  ; AX = NN0Fh | AH = NN | AL = 0F |
  ; CX = 90XXh | CH = 90 | CL = XX |
                                      ; Lo Hi
  mov word [InstructionBlock + 0], ax ; 0F NN
  mov word [InstructionBlock + 2], cx ; XX 90
  jmp @ExecuteInstruction

@NumberIsGreaterThan7:
  if defined x64
    mov al, cl
    mov cl, 8 ; �����, ������������ ����� ��������, ���� �� ��������� 8
    mul cl
    add al, 0C0h ; CH = 0xC0 + ByteNumber * 8 - ����, ������������ ����� ��������
    mov cl, al
    xchg ah, al
    mov ax, 0F44h  
    
    ; AX = 0F44h | AH = 0F | AL = 44 |
    ; CX = XXNNh | CH = XX | CL = NN |
                                        ; Lo Hi
    mov word [InstructionBlock + 0], ax ; 44 0F
    mov word [InstructionBlock + 2], cx ; NN XX
  else
    xchg ch, cl
    shl cx, 16
    mov cx, 0FF0h ; ECX = XXNN0FF0h
    mov eax, ecx
    mov dword [InstructionBlock], eax ; F0 0F NN XX
  end if

@ExecuteInstruction:
  
  if defined x64 
    mov rax, rdx
  else
    mov eax, edx
  end if
  
  InstructionBlock db 4 dup 90h ; ����������� ����� ��� ����������
  
@ValidationFailed:
  ret
  
endp

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

proc IdtGdtTrOperation ; Operation: DWord, Data: Pointer
  mov dword [IdtGdtTrInstructionBlock], ecx
  IdtGdtTrInstructionBlock db 4 dup 90h
  ret
endp