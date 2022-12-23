import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

public class MetaspaceDumpParser {
    static final int HPROF_UTF8 = 0x01;
    static final int HPROF_INSTANCE_KLASS = 0x0F;
    static final int HPROF_ARRAY_KLASS = 0x10;
    static final int HPROF_METHOD = 0x11;
    static Map<Long, String> symbols = new HashMap<>();
    private static Date createDate;

    public static void main(String[] args) throws Exception {
        HprofInputStream his = new HprofInputStream(new FileInputStream("../" + args[0]));
        int[] h;
        try {
            do {
                h = his.readHprofHeader();
                if (h == null) {
                    break;
                }
                if (h[0] == HPROF_UTF8) {
                    his.readSymbol(h[1]);
                } else if (h[0] == HPROF_INSTANCE_KLASS) {
                    his.readInstanceKlass(h[1]);
                } else if (h[0] == HPROF_ARRAY_KLASS) {
                    his.readArrayKlass(h[1]);
                } else if (h[0] == HPROF_METHOD) {
                    his.readMethod(h[1]);
                } else {
                    break;
                }
            } while (true);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    static class HprofInstanceClass {
        int accessFlags;
        String className;
        String superClassName;
        String[] interfaces;
        String[][] fields;
        String[] methods;

        public String toString() {
            return "" + className + " extends " + superClassName
                + " implements " + Arrays.toString(interfaces);
        }
    }

    static class HprofArrayClass {
        int accessFlags;
        String className;
        String superClassName;
        int dimension;

        @Override
        public String toString() {
            return className + "[" + dimension + "]";
        }
    }

    static class HprofMethod {
        String name;
        String signature;
        int accessFlags;
        int maxStack;
        int maxLocal;
        int paramSize;
        byte[] bytecodes;
    }

    static class HprofInputStream extends DataInputStream {
        private int oopSize;

        public HprofInputStream(InputStream in) throws Exception {
            super(in);

            final String header = "JAVA PROFILE 1.0.3";
            int headerLen = header.getBytes(StandardCharsets.US_ASCII).length;
            byte[] buf = new byte[headerLen + 1];
            int br = read(buf, 0, buf.length);
            if (!new String(buf).startsWith(header) || br != headerLen + 1) {
                throw new Exception("Invalid header " + new String(buf));
            }
            System.out.println(new String(buf));
            oopSize = readInt();
            System.out.println("== OopSize: " + oopSize);
            long timestamp = readLong();
            createDate = new Date(timestamp);
            System.out.println("== CreateDate: " + createDate);
        }

        int[] readHprofHeader() {
            try {
                int type = readByte();
                if (type == -1) {
                    // EOF
                    return null;
                }
                int tick = readInt(); // current ticks, should be 0
                if (tick != 0) {
                    throw new Exception("Unexpect ticks" + type);
                }
                int len = readInt();
                return new int[]{type, len};
            } catch (Exception e) {
                e.printStackTrace();
                return null;
            }
        }

        long readSymbolId() throws IOException {
            if (oopSize == 8) {
                return readLong();
            } else {
                return readInt();
            }
        }

        byte[] readRaw(int len) throws Exception {
            byte[] raw = new byte[len];
            int br = read(raw, 0, raw.length);
            if (br != len) {
                throw new Exception("expect to read " + len + " bytes but got " + br + " bytes");
            }
            return raw;
        }

        void readSymbol(int len) throws Exception {
            long addr = readSymbolId();
            byte[] raw = readRaw(len - oopSize);
            System.out.println("== Symbol: " + addr + " " + new String(raw, StandardCharsets.UTF_8));
            symbols.put(addr, new String(raw, StandardCharsets.UTF_8));
        }

        void readInstanceKlass(int len) throws Exception {
            HprofInstanceClass c = new HprofInstanceClass();
            c.accessFlags = readInt();
            c.className = symbols.get(readSymbolId());
            c.superClassName = symbols.get(readSymbolId());
            int numOfInterfaces = readInt();
            c.interfaces = new String[numOfInterfaces];
            for (int i = 0; i < numOfInterfaces; i++) {
                c.interfaces[i] = symbols.get(readSymbolId());
            }
            int numOfFields = readInt();
            c.fields = new String[numOfFields][2];
            for (int i = 0; i < numOfFields; i++) {
                String[] field = new String[2];
                field[0] = symbols.get(readSymbolId());
                field[1] = String.valueOf(readByte());
                c.fields[i] = field;
            }
            int numOfMethods = readInt();
            c.methods = new String[numOfMethods];
            for (int i = 0; i < numOfMethods; i++) {
                c.methods[i] = symbols.get(readSymbolId());
            }
           System.out.println("== Class: " + c);
        }

        void readArrayKlass(int len) throws Exception {
            HprofArrayClass c = new HprofArrayClass();
            c.accessFlags = readInt();
            c.className = symbols.get(readSymbolId());
            c.superClassName = symbols.get(readSymbolId());
            c.dimension = readInt();
            System.out.println("== ArrayClass: " + c);
        }

        void readMethod(int len) throws Exception {
            HprofMethod m = new HprofMethod();
            m.name = symbols.get(readSymbolId());
            m.signature = symbols.get(readSymbolId());
            m.accessFlags = readInt();
            m.maxStack = readInt();
            m.maxLocal = readInt();
            m.paramSize = readInt();
            m.bytecodes = new byte[readInt()];
            for (int i = 0; i < m.bytecodes.length; i++) {
                m.bytecodes[i] = readByte();
            }
           System.out.println("== Method: " + m);
        }
    }
}
