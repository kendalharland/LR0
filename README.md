LR0
=====

Generate the set of LR(0) items from an augmented grammar

####compling:
<pre>
	make
</pre>

####runnning:
```
	./lrz < grammar.txt
```

###example grammar:
```
	E
	E->E+T
	E->T
	T->T*F
	T->F
	F->(E)
	F->i
```

###example output:
<pre>
	Augmented Grammar
	-----------------
	'->E
	E->E+T
	E->T
	T->T*F
	T->F
	F->(E)
	F->i

	Sets of LR(0) Items
	-------------------
	I0:
		'->@E                goto(E)=I1
		E->@E+T
		E->@T                goto(T)=I2
		T->@T*F
		T->@F                goto(F)=I3
		F->@(E)              goto(()=I4
		F->@i                goto(i)=I5
	I1:
		'->E@
		E->E@+T              goto(+)=I6
	I2:
		E->T@
		T->T@*F              goto(*)=I7
	I3:
		T->F@
	I4:
		F->(@E)              goto(E)=I8
		E->@E+T
		E->@T                goto(T)=I2
		T->@T*F
		T->@F                goto(F)=I3
		F->@(E)              goto(()=I4
		F->@i                goto(i)=I5
	I5:
		F->i@
	I6:
		E->E+@T              goto(T)=I9
		T->@T*F
		T->@F                goto(F)=I3
		F->@(E)              goto(()=I4
		F->@i                goto(i)=I5
	I7:
		T->T*@F              goto(F)=I10
		F->@(E)              goto(()=I4
		F->@i                goto(i)=I5
	I8:
		F->(E@)              goto())=I11
		E->E@+T              goto(+)=I6
	I9:
		E->E+T@
		T->T@*F              goto(*)=I7
	I10:
		T->T*F@
	I11:
		F->(E)@
</pre>
