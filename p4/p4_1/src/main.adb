with GNAT.IO;  use GNAT.IO;
-- A�adir el paquete paquetebarrera
with paquetebarrera; use paquetebarrera;

procedure Main is
   -- Definir una constante NumTareas para indicar el n�mero de tareas que se bloquear�n en la barrera
   NumTareas : constant integer := 10;

   -- Definir el tipo PBarrera_t de tipo puntero a Barrera_t
   type PBarrera_t is access all Barrera_t;

   -- Definir una variable B de tipo Barrera_t permitiendo que se acceda a su direcci�n de memoria (aliased). Usaremos NumTareas como par�metro constructor
   B : aliased Barrera_t(NumTareas);

   -- Dejinir el tipo tarea TareaEnBarrera con un par�metro B de tipo PBarrera_t
   task type TareaEnBarrera(B : PBarrera_t);

   -- Implementar el cuerpo del tipo tarea
   task body TareaEnBarrera is begin

   -- Mostrar el mensaje Entrando en la barrera
   put_line("Entrando en la barrera");

   -- Invocar al procedimiento SincronizarEnBarrera pasando B.all como par�metro (B es un puntero a Barrera_t y el procedimiento recibe un Barrera_t)
   SincronizarEnBarrera(B.all);

   -- Mostrar el mensaje Saliendo de la barrera
   put_line("Saliendo de la barrera");
   end;
   -- Definir un array de tipo TareaEnBarrera con NumTareas elementos. Usar la direcci�n de B ('Access) como par�metro de TareaEnBarrera
   T : array(1 .. NumTareas) of TareaEnBarrera(B'Access);
begin
   null;
end Main;
