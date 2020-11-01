with GNAT.IO;  use GNAT.IO;
-- Añadir el paquete paquetebarrera
with paquetebarrera; use paquetebarrera;

procedure Main is
   -- Definir una constante NumTareas para indicar el número de tareas que se bloquearán en la barrera
   NumTareas : constant integer := 10;

   -- Definir el tipo PBarrera_t de tipo puntero a Barrera_t
   type PBarrera_t is access all Barrera_t;

   -- Definir una variable B de tipo Barrera_t permitiendo que se acceda a su dirección de memoria (aliased). Usaremos NumTareas como parámetro constructor
   B : aliased Barrera_t(NumTareas);

   -- Dejinir el tipo tarea TareaEnBarrera con un parámetro B de tipo PBarrera_t
   task type TareaEnBarrera(B : PBarrera_t);

   -- Implementar el cuerpo del tipo tarea
   task body TareaEnBarrera is begin

   -- Mostrar el mensaje Entrando en la barrera
   put_line("Entrando en la barrera");

   -- Invocar al procedimiento SincronizarEnBarrera pasando B.all como parámetro (B es un puntero a Barrera_t y el procedimiento recibe un Barrera_t)
   SincronizarEnBarrera(B.all);

   -- Mostrar el mensaje Saliendo de la barrera
   put_line("Saliendo de la barrera");
   end;
   -- Definir un array de tipo TareaEnBarrera con NumTareas elementos. Usar la dirección de B ('Access) como parámetro de TareaEnBarrera
   T : array(1 .. NumTareas) of TareaEnBarrera(B'Access);
begin
   null;
end Main;
