declare global {
    interface Window {
      Module : {
        preRun: never[];
        postRun: never[];
        print: void;
        canvas: HTMLElement | null;
        setStatus: (text: string) => void;
        totalDependencies: number;
        monitorRunDependencies: (left: number) => void;
        ccall: (functionName: string, returnType: string, argumentTypes: string[], arguments: any[]) => any;
    }
    }
  }

export {}