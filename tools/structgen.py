#!/usr/bin/env python3
# SPDX-License-Identifier: MIT

import sys, re, argparse



class Field:
    def __init__(self, name, bitpos, width, desc=None):
        self.name   = name
        self.bitpos = int(bitpos)
        self.width  = int(width)
        self.desc   = desc
    
    def __repr__(self):
        return "Field{{name={},bitpos={},width={},desc={}}}".format(repr(self.name), self.bitpos, self.width, repr(self.desc))
    
    def __eq__(self, other: object) -> bool:
        return type(other) == Field and other.name == self.name and other.bitpos == self.bitpos and other.width == self.width and other.desc == self.desc



class Register:
    def __init__(self, name, offset, desc=None):
        self.name   = name
        self.offset = int(offset)
        self.desc   = desc
        self.fields = []
    
    def __repr__(self):
        return "Register{{name={},offset={},desc={},fields={}}}".format(repr(self.name), self.offset, repr(self.desc), repr(self.fields))
    
    def add_field(self, field):
        if type(field) != Field:
            raise ValueError("Not a Field: " + repr(field))
        self.fields += [field]
        self.fields.sort(key=lambda x: x.bitpos)
    
    def collapsible(self):
        return len(self.fields) == 1
    
    def simple(self):
        return len(self.fields) == 0 or (len(self.fields) == 1 and self.fields[0].width == 32)
    
    def _generate_type(self, indent, name_prefix):
        out  = ""
        bitpos  = 0
        for field in self.fields:
            if field.bitpos > bitpos:
                out += indent+"uint32_t : {};\n".format(field.bitpos - bitpos)
                bitpos   = field.bitpos
            if field.desc:
                out += indent+"// {}\n".format(field.desc)
            out += indent+"uint32_t {} : {};\n".format(name_prefix + field.name, field.width)
            bitpos += field.width
        if bitpos < 32:
            out += indent+"uint32_t : {};\n".format(32 - bitpos)
        return out
    
    def generate_type(self, type_prefix="", length=None):
        if self.collapsible() and length == None: return ""
        if self.simple(): return ""
        out  = ""
        if self.desc:
            out  = "// {}\n".format(self.desc)
        out += "typedef volatile struct {\n"
        out += self._generate_type("    ", "")
        out += "}} {}{}_t;\n\n".format(type_prefix, self.name)
        return out
    
    def generate(self, type_prefix="", length=None):
        out = ""
        arr = "[{}]".format(length) if length != None else ""
        if self.simple():
            if len(self.fields) and self.fields[0].desc:
                out += "    // {}\n".format(self.fields[0].desc)
            out += "    volatile uint32_t {}{};\n".format(self.name, arr)
        elif self.collapsible() and length == None:
            if self.desc:
                out  = "    // {}\n".format(self.desc)
            out += "    volatile struct {\n"
            out += self._generate_type("        ", self.name+"_")
            out += "    };\n"
        else:
            if self.desc:
                out  = "    // {}\n".format(self.desc)
            out += "    {}{}_t {}{};\n".format(type_prefix, self.name, self.name, arr)
        return out



class Peripheral:
    def __init__(self, name, desc=""):
        self.name = name
        self.desc = desc
        self.regs = []
    
    def __repr__(self):
        return "Peripheral{{name={},desc={},regs={}}}".format(repr(self.name), repr(self.desc), repr(self.regs))
    
    def add_reg(self, reg):
        if type(reg) != Register:
            raise ValueError("Not a Register: " + repr(reg))
        self.regs += [reg]
        self.regs.sort(key=lambda x: x.offset)
    
    def generate(self) -> str:
        types = ""
        out   = ""
        if self.desc:
            out = "// {}\n".format(self.desc)
        out  += "typedef struct {\n"
        off   = 0
        padding = 0
        
        def create_array(reg: Register, length: int) -> str:
            nonlocal types, out, self
            # Create a copy of register.
            m = re.match("([a-zA-Z_]+)([0-9]+)(\w+)?", reg.name)
            template = Register(m.group(1)+(m.group(3) or ""), reg.offset, reg.desc)
            for field in reg.fields:
                m = re.match("([a-zA-Z_]+)([0-9]+)?(\w+)?", field.name)
                template.add_field(Field(m.group(1)+(m.group(3) or ""), field.bitpos, field.width, field.desc))
            
            # Output type just once.
            types += template.generate_type(self.name + "_", length)
            out   += template.generate(self.name + "_", length)
        
        i = 0
        while i < len(self.regs):
            reg = self.regs[i]
            
            # Validate registers.
            if reg.offset < off or reg.offset % 4 != 0:
                raise ValueError("Invalid offset in " + repr(reg))
            
            # Add any required padding.
            if off < reg.offset:
                out += "    // Padding.\n"
                out += "    volatile uint32_t _padding{}[{}];\n".format(padding, (reg.offset-off)//4)
                padding += 1
                off      = reg.offset
            
            if len(self.regs) > i+1 and can_create_array(reg, self.regs[i+1]):
                # Register array definitions.
                x = 2
                while i+x < len(self.regs) and can_create_array(reg, self.regs[i+x]): x += 1
                print("Array: ", x)
                create_array(reg, x)
                off += 4*x
                i   += x
            else:
                # Add register definitions.
                types += reg.generate_type(self.name + "_")
                out   += reg.generate(self.name + "_")
                off   += 4
                i     += 1
        
        out  += "} " + self.name + "_t;\n\n"
        return types+out



def can_create_array(a: Register, b: Register) -> bool:
    # Check field match.
    if len(a.fields) != len(b.fields): return False
    for i in range(len(a.fields)):
        ma = re.match("([a-zA-Z_]+)([0-9]+)?(\w+)?", a.fields[i].name)
        mb = re.match("([a-zA-Z_]+)([0-9]+)?(\w+)?", b.fields[i].name)
        if not ma or not mb: return False
        if ma.group(1) != mb.group(1) or ma.group(3) != mb.group(3):
            return False
    
    # Check name match.
    ma = re.match("([a-zA-Z_]+)([0-9]+)(\w+)?", a.name)
    mb = re.match("([a-zA-Z_]+)([0-9]+)(\w+)?", b.name)
    if not ma or not mb: return False
    ia = int(ma.group(2))
    ib = int(mb.group(2))
    if ib-ia != (b.offset-a.offset) // 4: return False
    
    if ma.group(1) != mb.group(1) or ma.group(3) != mb.group(3): return False
    return True



def parse(raw, name, desc=None, prefix="") -> Peripheral:
    """Parse an espressif register definition file."""
    peri = Peripheral(name, desc)
    
    # Convert line endings.
    raw = raw.replace("\r\n", "\n").replace("\r", "\n")
    
    while len(raw):
        # Look for two consecutive newlines.
        if "\n\n" not in raw: break
        raw = raw[raw.index("\n\n")+2:]
        
        # Register description comment.
        m_desc = re.match("\n?/\\*+((?:.|\n)+?)\\*/\n", raw)
        if m_desc:
            raw = raw[m_desc.end():]
        
        # Register address definition.
        m_addr = re.match("\n?#define "+prefix+"(\\w+?)(?:_REG)?\\s+\\(\w+_BASE\\s*\\+\\s*(0x[0-9A-Fa-f]+)\\)\n", raw)
        if not m_addr:
            m_addr = re.match("\n?#define "+prefix+"(\\w+?)(?:_REG)?\\(\w+\\)\\s+\\(\\w+_BASE\\(\\w+\\)\\s*\\+\\s*(0x[0-9a-fA-F]+)\\)\n", raw)
        if not m_addr: continue
        raw = raw[m_addr.end():]
        
        reg = Register(m_addr.group(1).lower(), int(m_addr.group(2), 0), None)
        
        while True:
            # Field description comment.
            m_field = re.match("\n?/\\* "+prefix+"(\\w+).+?bitpos:\\[([0-9]+)(?::([0-9]+))?\\].+?\\*/\n", raw)
            if not m_field: break
            raw = raw[m_field.end():]
            m_desc  = re.match("\n?/\\*(?:description:)? ?((?:.|\n)+?).?\\*/\n", raw)
            if not m_desc: break
            raw = raw[m_desc.end():]
            
            if m_field.group(3):
                reg.add_field(Field(m_field.group(1).lower(), int(m_field.group(3)), int(m_field.group(2)) - int(m_field.group(3)) + 1, m_desc.group(1).replace("\n", "")))
            else:
                reg.add_field(Field(m_field.group(1).lower(), int(m_field.group(2)), 1, m_desc.group(1).replace("\n", "")))
            
            # Skip defines.
            while m := re.match("\n?#define\\s.+", raw):
                raw = raw[m.end():]
            
        peri.add_reg(reg)
    
    return peri



if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog="structgen.py", description="Espressif MMIO peripheral struct generator.")
    parser.add_argument("--desc", action="store")
    parser.add_argument("--prefix", action="store", default="")
    parser.add_argument("name")
    parser.add_argument("infile", nargs="?", default="/proc/self/fd/0")
    parser.add_argument("outfile", nargs="?", default="/proc/self/fd/1")
    args = parser.parse_args()
    
    infd  = open(args.infile,  "r")
    outfd = open(args.outfile, "w")
    peri  = parse(infd.read(), args.name, args.desc, args.prefix)
    outfd.write("\n")
    outfd.write("// Generated from ESP-IDF: https://github.com/espressif/esp-idf\n")
    outfd.write("// " + args.infile + "\n")
    outfd.write("\n")
    outfd.write("#include <stdint.h>\n")
    outfd.write("\n")
    outfd.write(peri.generate())
    infd.close()
    outfd.close()
    
    # if len(sys.argv) == 5:
    #     infd  = open(sys.argv[3], "r")
    #     outfd = open(sys.argv[4], "w")
    #     peri  = parse(infd.read(), sys.argv[1], sys.argv[2])
    #     outfd.write(peri.generate())
    #     infd.close()
    #     outfd.close()
    # elif len(sys.argv) == 4:
    #     infd  = open(sys.argv[3], "r")
    #     peri  = parse(infd.read(), sys.argv[1], sys.argv[2])
    #     print(peri.generate())
    #     infd.close()
    # elif len(sys.argv) == 3:
    #     peri  = parse(sys.stdin.read(), sys.argv[1], sys.argv[2])
    #     print(peri.generate())
    # else:
    #     print("Usage: structgen.py <name> [--desc=...] [--prefix=...] [infile] [outfile]")
    #     exit(1)
