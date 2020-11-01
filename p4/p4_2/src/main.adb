with GNAT.IO;use GNAT.IO;
-- Añadir el paquete controlador
with controlador;use controlador;


procedure Main is

   -- Definir el tipo tarea EsperarCtrlC con un parámetro esperadas (de tipo entero)
   task type EsperarCtrlC(esperadas : Integer);

   task body EsperarCtrlC is
      -- Declarar una variable para contar el número de ocurrencias de la señal SIGINT e inicializarla a 0
      nSig : Integer := 0;

   begin
      -- Mientras no se hayan recibido las señales esperadas
      while nSig < esperadas loop

         -- Invocar a la entrada Wait de ControladorSigInt para esperar a que se pulse Ctrl+C
         ControladorSigInt.Wait;

         -- Incrementar las señales recibidas
         nSig:= nSig+1;

         -- Fin mientras
         end loop;

   end EsperarCtrlC;
   -- Crear una tarea de tipo EsperarCtrlC pasando por parámetros el número de señales SIGINT que debe recibir
   A : EsperarCtrlC(5);

begin
   null;
end Main;
