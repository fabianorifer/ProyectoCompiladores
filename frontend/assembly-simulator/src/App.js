import React, { useState } from 'react';
import { Play, SkipForward, RotateCcw } from 'lucide-react';

const AssemblySimulator = () => {
  const defaultCode = `.data
print_fmt: .string "%ld \\n"
.text
.globl main
main:
pushq %rbp
movq %rsp, %rbp
subq $32, %rsp
movq $2, %rax
movq %rax, -8(%rbp)
movq $5, %rax
movq %rax, -24(%rbp)
movq $1, %rax
movq %rax, -32(%rbp)
while_0:
movq $0, %rax
pushq %rax
movq -24(%rbp), %rax
movq %rax, %rcx
popq %rax
cmpq %rcx, %rax
movl $0, %eax
setl %al
movzbq %al, %rax
cmpq $0, %rax
je endwhile_0
movq -32(%rbp), %rax
pushq %rax
movq -8(%rbp), %rax
movq %rax, %rcx
popq %rax
imulq %rcx, %rax
movq %rax, -32(%rbp)
movq -24(%rbp), %rax
pushq %rax
movq $1, %rax
movq %rax, %rcx
popq %rax
subq %rcx, %rax
movq %rax, -24(%rbp)
jmp while_0
endwhile_0:
movq -32(%rbp), %rax
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq $0, %rax
jmp .end_main
.end_main:
leave
ret`;

  const [code, setCode] = useState(defaultCode);
  const [currentLine, setCurrentLine] = useState(-1);
  const [registers, setRegisters] = useState({
    rax: '',
    rcx: '',
    rsi: '',
    rdi: '',
    rdx: ''
  });
  const [stack, setStack] = useState({});
  const [rbp, setRbp] = useState(0);
  const [rsp, setRsp] = useState(0);
  const [tempStack, setTempStack] = useState([]);
  const [flags, setFlags] = useState({ zero: false, sign: false, overflow: false, carry: false });

  const getAllLines = () => {
    return code.split('\n').map(l => l.trim()).filter(l => l);
  };

  const getExecutableLines = () => {
    return code.split('\n').filter(l => {
      const trimmed = l.trim();
      return trimmed && !trimmed.startsWith('.') && !trimmed.endsWith(':');
    });
  };

  const findLabelLine = (label) => {
    const allLines = getAllLines();
    for (let i = 0; i < allLines.length; i++) {
      if (allLines[i] === label + ':' || allLines[i] === '.' + label + ':') {
        const executableLines = getExecutableLines();
        let execIndex = 0;
        const codeLines = code.split('\n');
        for (let j = 0; j <= i; j++) {
          const trimmed = codeLines[j].trim();
          if (trimmed && !trimmed.startsWith('.') && !trimmed.endsWith(':')) {
            execIndex++;
          }
        }
        return execIndex;
      }
    }
    return -1;
  };

  const parseInstruction = (line) => {
    const trimmed = line.trim();
    const parts = trimmed.split(/\s+/);
    return {
      instruction: parts[0],
      operands: parts.slice(1).join(' ').split(',').map(op => op.trim())
    };
  };

  const executeInstruction = (lineIndex) => {
    const lines = getExecutableLines();
    if (lineIndex >= lines.length) return lineIndex;

    const line = lines[lineIndex];
    const parsed = parseInstruction(line);
    const instruction = parsed.instruction;
    const operands = parsed.operands;

    let newRegisters = { ...registers };
    let newStack = { ...stack };
    let newRbp = rbp;
    let newRsp = rsp;
    let newTempStack = [...tempStack];
    let newFlags = { ...flags };
    let nextLine = lineIndex + 1;

    const getRegisterName = (operand) => {
      const reg = operand.replace('%', '');
      const mapping = {
        'eax': 'rax', 
        'ebx': 'rbx', 
        'ecx': 'rcx', 
        'edx': 'rdx',
        'esi': 'rsi', 
        'edi': 'rdi', 
        'al': 'rax', 
        'bl': 'rbx',
        'cl': 'rcx', 
        'dl': 'rdx'
      };
      return mapping[reg] || reg;
    };

    const getValue = (operand) => {
      if (!operand) return 0;
      
      if (operand.startsWith('$')) {
        return parseInt(operand.substring(1));
      } else if (operand.startsWith('%')) {
        const regName = getRegisterName(operand);
        return newRegisters[regName] || 0;
      } else if (operand.includes('(%rbp)') || operand.includes('(%rsp)')) {
        const match = operand.match(/-?\d+/);
        const offset = match ? parseInt(match[0]) : 0;
        return newStack[offset] || 0;
      }
      return 0;
    };

    const setValue = (operand, value) => {
      if (!operand) return;
      
      if (operand.startsWith('%')) {
        const regName = getRegisterName(operand);
        if (['rax', 'rcx', 'rsi', 'rdi', 'rdx'].includes(regName)) {
          newRegisters[regName] = value;
        }
      } else if (operand.includes('(%rbp)') || operand.includes('(%rsp)')) {
        const match = operand.match(/-?\d+/);
        const offset = match ? parseInt(match[0]) : 0;
        newStack[offset] = value;
      }
    };

    switch (instruction) {
      case 'pushq':
        if (operands[0] === '%rbp') {
          newRbp = 0;
          newStack[0] = 'RBP_OLD';
        } else {
          const val = getValue(operands[0]);
          newTempStack.push(val);
          newRsp -= 8;
        }
        break;

      case 'movq':
      case 'movl':
      case 'movzbq':
      case 'movzbl':
        const value = getValue(operands[0]);
        setValue(operands[1], value);
        break;

      case 'addq':
      case 'addl':
        const addVal = getValue(operands[0]);
        if (operands[1] === '%rsp') {
          newRsp += addVal;
        } else {
          const current = getValue(operands[1]);
          const result = current + addVal;
          setValue(operands[1], result);
          newFlags.zero = result === 0;
          newFlags.sign = result < 0;
          // Overflow si ambos operandos tienen el mismo signo y el resultado tiene signo diferente
          newFlags.overflow = ((current >= 0 && addVal >= 0 && result < 0) || 
                               (current < 0 && addVal < 0 && result >= 0));
          // Carry para suma sin signo (simplificado)
          newFlags.carry = result < current;
        }
        break;

      case 'subq':
      case 'subl':
        const subVal = getValue(operands[0]);
        if (operands[1] === '%rsp') {
          newRsp -= subVal;
        } else {
          const current = getValue(operands[1]);
          const result = current - subVal;
          setValue(operands[1], result);
          newFlags.zero = result === 0;
          newFlags.sign = result < 0;
          // Overflow si los operandos tienen signo diferente y el resultado tiene signo diferente al minuendo
          newFlags.overflow = ((current >= 0 && subVal < 0 && result < 0) || 
                               (current < 0 && subVal >= 0 && result >= 0));
          // Carry (borrow) para resta sin signo
          newFlags.carry = current < subVal;
        }
        break;

      case 'popq':
        if (newTempStack.length > 0) {
          const val = newTempStack.pop();
          setValue(operands[0], val);
          newRsp += 8;
        }
        break;

      case 'imulq':
      case 'imull':
        if (operands.length === 2) {
          const mulVal = getValue(operands[0]);
          const current = getValue(operands[1]);
          setValue(operands[1], current * mulVal);
        } else {
          const mulVal = getValue(operands[0]);
          newRegisters.rax = (newRegisters.rax || 0) * mulVal;
        }
        break;

      case 'idivq':
      case 'divq':
        const divOperand = operands[0];
        const divisor = getValue(divOperand);
        if (divisor !== 0) {
          const dividend = newRegisters.rax || 0;
          newRegisters.rax = Math.floor(dividend / divisor);
          newRegisters.rdx = dividend % divisor;
        }
        break;

      case 'cqto':
      case 'cqo':
        const raxValue = newRegisters.rax || 0;
        newRegisters.rdx = raxValue < 0 ? -1 : 0;
        break;

      case 'cmpq':
      case 'cmpl':
        const cmpVal1 = getValue(operands[1]);
        const cmpVal2 = getValue(operands[0]);
        const cmpResult = cmpVal1 - cmpVal2;
        newFlags.zero = cmpResult === 0;
        newFlags.sign = cmpResult < 0;
        newFlags.overflow = ((cmpVal1 >= 0 && cmpVal2 < 0 && cmpResult < 0) || 
                            (cmpVal1 < 0 && cmpVal2 >= 0 && cmpResult >= 0));
        newFlags.carry = cmpVal1 < cmpVal2;
        break;

      case 'testq':
      case 'testl':
        const testVal1 = getValue(operands[0]);
        const testVal2 = getValue(operands[1]);
        const testResult = testVal1 & testVal2;
        newFlags.zero = testResult === 0;
        newFlags.sign = testResult < 0;
        newFlags.overflow = false;
        newFlags.carry = false;
        break;

      case 'andq':
      case 'andl':
        const andVal = getValue(operands[0]);
        const andCurrent = getValue(operands[1]);
        const andResult = andCurrent & andVal;
        setValue(operands[1], andResult);
        newFlags.zero = andResult === 0;
        newFlags.sign = andResult < 0;
        newFlags.overflow = false;
        newFlags.carry = false;
        break;

      case 'orq':
      case 'orl':
        const orVal = getValue(operands[0]);
        const orCurrent = getValue(operands[1]);
        const orResult = orCurrent | orVal;
        setValue(operands[1], orResult);
        newFlags.zero = orResult === 0;
        newFlags.sign = orResult < 0;
        newFlags.overflow = false;
        newFlags.carry = false;
        break;

      case 'xorq':
      case 'xorl':
      case 'xor':
        const xorVal = getValue(operands[0]);
        const xorCurrent = getValue(operands[1]);
        const xorResult = xorCurrent ^ xorVal;
        setValue(operands[1], xorResult);
        newFlags.zero = xorResult === 0;
        newFlags.sign = xorResult < 0;
        newFlags.overflow = false;
        newFlags.carry = false;
        break;

      case 'setl':
        setValue(operands[0], newFlags.sign !== newFlags.overflow ? 1 : 0);
        break;
      case 'setg':
        setValue(operands[0], (!newFlags.zero && newFlags.sign === newFlags.overflow) ? 1 : 0);
        break;
      case 'sete':
        setValue(operands[0], newFlags.zero ? 1 : 0);
        break;
      case 'setne':
        setValue(operands[0], !newFlags.zero ? 1 : 0);
        break;
      case 'setle':
        setValue(operands[0], (newFlags.zero || newFlags.sign !== newFlags.overflow) ? 1 : 0);
        break;
      case 'setge':
        setValue(operands[0], newFlags.sign === newFlags.overflow ? 1 : 0);
        break;
      case 'setb':
        setValue(operands[0], newFlags.carry ? 1 : 0);
        break;
      case 'setbe':
        setValue(operands[0], (newFlags.carry || newFlags.zero) ? 1 : 0);
        break;
      case 'seta':
        setValue(operands[0], (!newFlags.carry && !newFlags.zero) ? 1 : 0);
        break;
      case 'setae':
        setValue(operands[0], !newFlags.carry ? 1 : 0);
        break;

      case 'leaq':
        if (operands[1]) {
          const regName = getRegisterName(operands[1]);
          newRegisters[regName] = operands[0].replace(/[()%]/g, '');
        }
        break;

      case 'jmp':
        const jmpLabel = operands[0];
        const jmpLine = findLabelLine(jmpLabel);
        if (jmpLine !== -1) {
          nextLine = jmpLine;
        }
        break;

      case 'je':
        if (newFlags.zero) {
          const jeLabel = operands[0];
          const jeLine = findLabelLine(jeLabel);
          if (jeLine !== -1) {
            nextLine = jeLine;
          }
        }
        break;

      case 'jne':
        if (!newFlags.zero) {
          const jneLabel = operands[0];
          const jneLine = findLabelLine(jneLabel);
          if (jneLine !== -1) {
            nextLine = jneLine;
          }
        }
        break;

      case 'jg':
        if (!newFlags.zero && newFlags.sign === newFlags.overflow) {
          const jgLabel = operands[0];
          const jgLine = findLabelLine(jgLabel);
          if (jgLine !== -1) {
            nextLine = jgLine;
          }
        }
        break;

      case 'jl':
        if (newFlags.sign !== newFlags.overflow) {
          const jlLabel = operands[0];
          const jlLine = findLabelLine(jlLabel);
          if (jlLine !== -1) {
            nextLine = jlLine;
          }
        }
        break;

      case 'jge':
        if (newFlags.sign === newFlags.overflow) {
          const jgeLabel = operands[0];
          const jgeLine = findLabelLine(jgeLabel);
          if (jgeLine !== -1) {
            nextLine = jgeLine;
          }
        }
        break;

      case 'jle':
        if (newFlags.zero || newFlags.sign !== newFlags.overflow) {
          const jleLabel = operands[0];
          const jleLine = findLabelLine(jleLabel);
          if (jleLine !== -1) {
            nextLine = jleLine;
          }
        }
        break;

      case 'ja':
        if (!newFlags.carry && !newFlags.zero) {
          const jaLabel = operands[0];
          const jaLine = findLabelLine(jaLabel);
          if (jaLine !== -1) {
            nextLine = jaLine;
          }
        }
        break;

      case 'jb':
        if (newFlags.carry) {
          const jbLabel = operands[0];
          const jbLine = findLabelLine(jbLabel);
          if (jbLine !== -1) {
            nextLine = jbLine;
          }
        }
        break;

      case 'jae':
        if (!newFlags.carry) {
          const jaeLabel = operands[0];
          const jaeLine = findLabelLine(jaeLabel);
          if (jaeLine !== -1) {
            nextLine = jaeLine;
          }
        }
        break;

      case 'jbe':
        if (newFlags.carry || newFlags.zero) {
          const jbeLabel = operands[0];
          const jbeLine = findLabelLine(jbeLabel);
          if (jbeLine !== -1) {
            nextLine = jbeLine;
          }
        }
        break;

      case 'leave':
        newRsp = 0;
        newRbp = 0;
        newStack = {};
        break;

      case 'ret':
      case 'call':
        break;
    }

    setRegisters(newRegisters);
    setStack(newStack);
    setRbp(newRbp);
    setRsp(newRsp);
    setTempStack(newTempStack);
    setFlags(newFlags);
    
    return nextLine;
  };

  const stepForward = () => {
    const lines = getExecutableLines();
    if (currentLine + 1 < lines.length) {
      const lineToExecute = currentLine + 1;
      const nextLine = executeInstruction(lineToExecute);
      setCurrentLine(lineToExecute);
      setTimeout(() => {
        setCurrentLine(nextLine - 1);
      }, 0);
    }
  };

  const reset = () => {
    setCurrentLine(-1);
    setRegisters({ rax: '', rcx: '', rsi: '', rdi: '', rdx: '' });
    setStack({});
    setRbp(0);
    setRsp(0);
    setTempStack([]);
    setFlags({ zero: false, sign: false, overflow: false, carry: false });
  };

  const runAll = () => {
    reset();
    const lines = getExecutableLines();
    let line = 0;
    let iterations = 0;
    const maxIterations = 10000;
    let lastExecutedLine = -1;
    
    while (line < lines.length && iterations < maxIterations) {
      lastExecutedLine = line;
      line = executeInstruction(line);
      iterations++;
    }
    
    setCurrentLine(lastExecutedLine);
  };

  const codeLines = getExecutableLines();
  const stackOffsets = [0, -8, -16, -24, -32];

  return (
    <div className="min-h-screen bg-gray-900 text-white p-6">
      <h1 className="text-3xl font-bold mb-6 text-center">Simulador x86-64 Assembly</h1>
      
      <div className="grid grid-cols-1 lg:grid-cols-3 gap-6 mb-6">
        <div className="lg:col-span-2 bg-gray-800 rounded-lg p-4">
          <h2 className="text-xl font-semibold mb-3">Código Assembly</h2>
          <div className="bg-gray-900 rounded border border-gray-700 p-3 h-96 overflow-y-auto font-mono text-sm">
            {codeLines.map((line, idx) => (
              <div
                key={idx}
                className={`py-1 px-2 ${
                  idx === currentLine
                    ? 'bg-yellow-600 text-black font-bold'
                    : 'text-green-400'
                }`}
              >
                <span className="text-gray-500 mr-3">{idx + 1}</span>
                {line}
              </div>
            ))}
          </div>
          <div className="mt-4">
            <textarea
              value={code}
              onChange={(e) => {
                setCode(e.target.value);
                reset();
              }}
              className="w-full h-32 bg-gray-900 text-green-400 font-mono p-3 rounded border border-gray-700 focus:outline-none focus:border-blue-500"
              placeholder="Pega tu código assembly aquí..."
              spellCheck="false"
            />
          </div>
          <div className="flex gap-3 mt-4">
            <button
              onClick={stepForward}
              className="flex items-center gap-2 bg-blue-600 hover:bg-blue-700 px-4 py-2 rounded transition"
            >
              <SkipForward size={18} />
              Siguiente Línea
            </button>
            <button
              onClick={runAll}
              className="flex items-center gap-2 bg-green-600 hover:bg-green-700 px-4 py-2 rounded transition"
            >
              <Play size={18} />
              Ejecutar Todo
            </button>
            <button
              onClick={reset}
              className="flex items-center gap-2 bg-red-600 hover:bg-red-700 px-4 py-2 rounded transition"
            >
              <RotateCcw size={18} />
              Reiniciar
            </button>
          </div>
          <div className="mt-3 text-sm text-gray-400">
            {currentLine === -1 ? (
              <span>Presiona "Siguiente Línea" para comenzar</span>
            ) : (
              <span>Última línea ejecutada: {currentLine + 1} / {codeLines.length}</span>
            )}
          </div>
        </div>

        <div className="bg-gray-800 rounded-lg p-4">
          <h2 className="text-xl font-semibold mb-3">Registros</h2>
          <div className="space-y-2">
            {['rax', 'rcx', 'rdx', 'rsi', 'rdi'].map(reg => (
              <div key={reg} className="flex bg-yellow-600 rounded">
                <div className="w-20 font-bold p-3 uppercase border-r-2 border-yellow-700">
                  {reg}
                </div>
                <div className="flex-1 p-3 text-center font-mono">
                  {registers[reg] !== '' ? registers[reg] : '-'}
                </div>
              </div>
            ))}
          </div>
          <div className="mt-4">
            <h3 className="font-semibold mb-2">Flags</h3>
            <div className="space-y-1 text-sm">
              <div>Zero (ZF): {flags.zero ? '1' : '0'}</div>
              <div>Sign (SF): {flags.sign ? '1' : '0'}</div>
              <div>Overflow (OF): {flags.overflow ? '1' : '0'}</div>
              <div>Carry (CF): {flags.carry ? '1' : '0'}</div>
            </div>
          </div>
        </div>
      </div>

      <div className="bg-gray-800 rounded-lg p-4">
        <h2 className="text-xl font-semibold mb-3">PILA</h2>
        <div className="overflow-x-auto">
          <table className="w-full border-collapse">
            <thead>
              <tr className="bg-gray-700">
                <th className="border border-gray-600 p-2">Offset</th>
                <th className="border border-gray-600 p-2">Valor</th>
                <th className="border border-gray-600 p-2">Puntero</th>
              </tr>
            </thead>
            <tbody>
              {stackOffsets.map(offset => (
                <tr key={offset} className={offset === 0 ? 'bg-blue-900' : ''}>
                  <td className="border border-gray-600 p-2 text-center font-mono">
                    {offset}
                  </td>
                  <td className="border border-gray-600 p-2 text-center font-mono bg-yellow-600">
                    {stack[offset] !== undefined ? stack[offset] : '-'}
                  </td>
                  <td className="border border-gray-600 p-2 text-center">
                    {offset === 0 && 'RBP'}
                    {offset === rsp && rsp !== 0 && 'RSP'}
                  </td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
        {tempStack.length > 0 && (
          <div className="mt-4">
            <h3 className="font-semibold mb-2">Pila Temporal (pushq/popq):</h3>
            <div className="flex gap-2">
              {tempStack.map((val, idx) => (
                <div key={idx} className="bg-purple-600 px-3 py-2 rounded font-mono">
                  {val}
                </div>
              ))}
            </div>
          </div>
        )}
      </div>
    </div>
  );
};

export default AssemblySimulator;