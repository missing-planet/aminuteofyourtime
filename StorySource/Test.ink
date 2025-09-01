
== TestKnot 
    This is a bit of test dialogue. # name:Unknown
    I am the Devil's Advocate. # sprite:Advocate #bg:Rectangles
    How are you doing today? # name:Advocate 
        * [I am doing well.]
            That is good.
        * [Bad.]
            That's too bad.
    - And how are you feeling? 
        * [Good.]
            That is good.
        * [Bad.]
            That's too bad.
    - I see. Well I hope the rest of your day goes well.
    I am going to take my leave now. # bg:Hidden
    -> DONE
    
== TestEvent
    This is a test of an event.
    I am trying to test this one with a longer section.
    Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam suscipit ante sit amet porta fringilla. Maecenas bibendum luctus efficitur. Vestibulum posuere, libero sit amet tincidunt scelerisque, tellus odio interdum purus, vitae aliquet metus nisl eget purus. Nulla pharetra lacus rutrum, rhoncus sapien vel, lacinia velit.
    Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec sit amet eros ligula. Nam condimentum magna ac tincidunt placerat. Curabitur sollicitudin eros mi, nec gravida ex sodales a. Donec iaculis auctor convallis. Maecenas placerat nisl nibh, non venenatis lectus sollicitudin eget. 
    Mauris iaculis accumsan purus quis euismod. Fusce massa ligula, luctus ac lobortis ac, feugiat in elit. Nam tempus posuere nunc quis scelerisque. 
    Now a quick choice test.
        * [The first option] -> DONE
        * [The second option] -> DONE
        * [The evil third option]
    -> DONE
    
== IconTest
    Followed a strange squealing to the uncanny underpass of Limbo. Two naked, sexless husks flop against each other atop an exhausted pile of other followers.
    <img id="Icon1"/> <XL>FOR LOVE! FOR LOVE!</>
    <img id="Icon2"/> FOR LOVE! FOR LOVE!
    <img id="CardBack"/> gained ADVANCED KNOWLEDGE
    -> DONE